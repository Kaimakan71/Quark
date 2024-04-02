/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <error.h>
#include <hash.h>
#include <lexer/lexer.h>
#include <parser/type.h>
#include <parser/parser.h>

static ast_node_t* root;
static ast_node_t* strings;
static unsigned int next_string_id;

static void parse_parameters(token_t* token, ast_node_t* procedure)
{
        while (token->kind != TK_RPAREN) {
                ast_node_t* parameter;

                parameter = create_node(procedure);
                parameter->kind = NK_PARAMETER;
                parameter->flags |= NF_NAMED;

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected parameter type\n");
                        delete_node(parameter);
                        return;
                }

                parameter->type = find_node_of_kind(token, root, NK_BUILTIN_TYPE);
                if (parameter->type == NULL) {
                        delete_node(parameter);
                        return;
                }

                /* Allow pointer types for parameters */
                lexer_next(token);
                parameter->pointer_depth = 0;
                while (token->kind == TK_STAR) {
                        parameter->pointer_depth++;
                        lexer_next(token);
                }

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Parameter type must be followed by a parameter name\n");
                        delete_node(parameter);
                        return;
                }

                /* Prevent redeclarations */
                if (find_node(token, procedure) != NULL) {
                        error(token, "\"%.*s\" has already been declared\n", token->length, token->pos);
                        delete_node(parameter);
                        return;
                }

                parameter->name.string = token->pos;
                parameter->name.length = token->length;
                parameter->name.hash = token->hash;
                push_node(parameter);

                lexer_next(token);
                if (token->kind != TK_COMMA) {
                        return;
                }

                lexer_next(token);
                if (token->kind == TK_RPAREN) {
                        warn(token, "Extra \",\" after parameters\n");
                        return;
                }
        }
}

static void parse_arguments(token_t* token, ast_node_t* call)
{
        while (token->kind != TK_RPAREN) {
                ast_node_t* argument;

                argument = create_node(call);

                if (token->kind == TK_IDENTIFIER) {
                        argument->kind = NK_VARIABLE_REFERENCE;

                        argument->variable = find_node_of_kind(token, call->parent, NK_VARIABLE);
                        if (argument->variable == NULL) {
                                error(token, "\"%.*s\" has not been declared or is not a variable\n", token->length, token->pos);
                                delete_node(argument);
                                return;
                        }
                } else if (token->kind == TK_NUMBER) {
                        argument->kind = NK_NUMBER;
                        argument->value = token->value;
                } else if (token->kind == TK_STRING) {
                        ast_node_t* string;

                        argument->kind = NK_STRING_REFERENCE;
                        argument->string = create_node(strings);
                        argument->string->kind = NK_STRING;
                        argument->string->string_id = next_string_id++;
                        argument->string->string_length = token->length - 1;
                        argument->string->string_data = token->pos + 1;
                        push_node(argument->string);
                } else {
                        error(token, "Expected argument value or \")\"\n");
                        delete_node(argument);
                        return;
                }

                push_node(argument);
                lexer_next(token);
                if (token->kind != TK_COMMA) {
                        return;
                }

                lexer_next(token);
                if (token->kind == TK_RPAREN) {
                        warn(token, "Extra \",\" after arguments\n");
                        return;
                }
        }
}

static void parse_call(token_t* token, ast_node_t* caller, ast_node_t* callee)
{
        ast_node_t* call;

        call = create_node(caller);
        call->kind = NK_CALL;
        call->callee = callee;

        lexer_next(token);
        if (token->kind != TK_LPAREN) {
                error(token, "Expected \"(\"\n");
                delete_node(call);
                return;
        }

        lexer_next(token);
        parse_arguments(token, call);

        if (token->kind != TK_RPAREN) {
                error(token, "Argument list must be followed by a \")\"\n");
                delete_node(call);
                return;
        }

        push_node(call);
        lexer_next(token);
}

static void parse_assignment_value(token_t* token, ast_node_t* assignment)
{
        if (token->kind == TK_IDENTIFIER) {
                ast_node_t* callee;

                callee = find_node_of_kind(token, root, NK_PROCEDURE);
                if (callee == NULL) {
                        error(token, "\"%.*s\" has not been declared or is not a procedure\n", token->length, token->pos);
                        return;
                }

                parse_call(token, assignment, callee);
        } else {
                error(token, "\"=\" must be followed by a value\n");
                return;
        }
}

static void parse_local_declaration(token_t* token, ast_node_t* procedure, ast_node_t* scope, ast_node_t* type)
{
        ast_node_t* variable;

        variable = create_node(scope);
        variable->kind = NK_VARIABLE;
        variable->flags = NF_NAMED;
        variable->type = type;
        variable->local_offset = procedure->local_size;
        procedure->local_size += type->size;

        lexer_next(token);
        if (token->kind != TK_IDENTIFIER) {
                error(token, "Expected variable name\n");
                delete_node(variable);
                return;
        }
        variable->name.string = token->pos;
        variable->name.length = token->length;
        variable->name.hash = token->hash;

        /* Generate an assignment if the variable is initialized */
        lexer_next(token);
        if (token->kind == TK_EQUALS && !(token->flags & TF_EQUALS)) {
                ast_node_t* assignment;

                assignment = create_node(scope);
                assignment->kind = NK_ASSIGNMENT;
                assignment->destination = variable;

                lexer_next(token);
                parse_assignment_value(token, assignment);

                push_node(assignment);
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Variable declaration must be terminated with a \";\"\n");
                delete_node(variable);
                return;
        }

        push_node(variable);
        lexer_next(token);
}

static void parse_assignment(token_t* token, ast_node_t* scope, ast_node_t* variable)
{
        ast_node_t* assignment;
        ast_node_t* callee;

        if (variable->parent == root) {
                DEBUG("TODO: Allow assignment to global variables\n");
                error(token, "Cannot assign to global variable\n");
                return;
        }

        assignment = create_node(scope);
        assignment->kind = NK_ASSIGNMENT;
        assignment->destination = variable;

        lexer_next(token);
        if (token->kind != TK_EQUALS || token->flags & TF_EQUALS) {
                error(token, "Variable name must be followed by a \"=\"\n");
                return;
        }

        lexer_next(token);
        parse_assignment_value(token, assignment);

        if (token->kind != TK_SEMICOLON) {
                error(token, "Assignment must be terminated with a \";\"\n");
                delete_node(assignment);
                return;
        }

        push_node(assignment);
        lexer_next(token);
}

static void parse_procedure_body(token_t* token, ast_node_t* procedure)
{
        while (token->kind != TK_RCURLY) {
                ast_node_t* node;

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected identifier\n");
                        return;
                }

                node = find_node(token, procedure);
                if (node == NULL) {
                        error(token, "\"%.*s\" has not been declared\n", token->length, token->pos);
                        return;
                }

                if (node->kind == NK_PROCEDURE) {
                        parse_call(token, procedure, node);
                        if (token->kind != TK_SEMICOLON) {
                                error(token, "Call must be terminated with a \";\"\n");
                                return;
                        }

                        lexer_next(token);
                } else if (node->kind == NK_BUILTIN_TYPE) {
                        parse_local_declaration(token, procedure, procedure, node);
                } else if (node->kind == NK_VARIABLE) {
                        parse_assignment(token, procedure, node);
                } else {
                        error(token, "\"%.*s\" is not a procedure, type, or variable\n", token->length, token->pos);
                        return;
                }
        }
}

static void parse_procedure(token_t* token)
{
        ast_node_t* procedure;

        procedure = create_node(root);
        procedure->kind = NK_PROCEDURE;
        procedure->flags = NF_NAMED;
        procedure->local_size = 0;

        /* Procedure name */
        lexer_next(token);
        if (token->kind != TK_IDENTIFIER) {
                error(token, "\"proc\" must be followed by the procedure's name\n");
                delete_node(procedure);
                return;
        }
        procedure->name.string = token->pos;
        procedure->name.length = token->length;
        procedure->name.hash = token->hash;

        lexer_next(token);
        if (token->kind != TK_LPAREN) {
                error(token, "Procedure name must be followed by a \"(\"\n");
                delete_node(procedure);
                return;
        }

        /* Procedure parameters */
        lexer_next(token);
        parse_parameters(token, procedure);
        if (token->kind != TK_RPAREN) {
                error(token, "Parameter list must be followed by a \")\"\n");
                delete_node(procedure);
                return;
        }

        /* Return type */
        lexer_next(token);
        if (token->kind == TK_ARROW) {
                lexer_next(token);
                if (token->kind != TK_IDENTIFIER) {
                        error(token, "\"->\" must be followed by the procedure's return type\n");
                        return;
                }

                procedure->return_type = find_node_of_kind(token, root, NK_BUILTIN_TYPE);
                if (procedure->return_type == NULL) {
                        error(token, "\"%.*s\" has not been declared or is not a type\n", token->length, token->pos);
                        return;
                }

                lexer_next(token);
        } else {
                procedure->return_type = NULL;
        }

        /* Procedure declarations may be terminated by a ";" */
        if (token->kind == TK_SEMICOLON) {
                push_node(procedure);
                lexer_next(token);
                return;
        }

        /* If there is no semicolon, a "{" must start the procedure body */
        if (token->kind != TK_LCURLY) {
                error(token, "Procedure declaration must be terminated with a \";\" or \"{\"\n");
                delete_node(procedure);
                return;
        }

        /* Parse procedure body, if not empty */
        lexer_next(token);
        if (token->kind != TK_RCURLY) {
                procedure->flags |= NF_DEFINITION;
                parse_procedure_body(token, procedure);
                if (token->kind != TK_RCURLY) {
                        delete_node(procedure);
                        return;
                }
        }

        push_node(procedure);
        lexer_next(token);
}

ast_node_t* parse(char* source, ast_node_t** strings_out)
{
        token_t token;

        DEBUG("Parsing...\n");

        root = malloc(sizeof(ast_node_t));
        strings = malloc(sizeof(ast_node_t));
        memset(root, 0, sizeof(ast_node_t));
        memset(strings, 0, sizeof(ast_node_t));
        next_string_id = 0;

        init_types(root);

        lexer_init(source);
        lexer_next(&token);
        while (token.kind != TK_EOF) {
                if (token.kind == TK_PROC) {
                        parse_procedure(&token);
                        continue;
                }

                error(&token, "Unexpected \"%.*s\"\n", token.length, token.pos);
                break;
        }

        *strings_out = strings;
        return root;
}
