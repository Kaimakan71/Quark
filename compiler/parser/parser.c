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

static ast_node_t* parse_call(token_t* token, ast_node_t* parent, ast_node_t* callee);
static ast_node_t* parse_variable_reference(token_t* token, ast_node_t* parent, ast_node_t* variable);

static ast_node_t* parse_value(token_t* token, ast_node_t* parent)
{
        DEBUG("Parsing value...\n");

        if (token->kind == TK_IDENTIFIER) {
                ast_node_t* node;

                node = find_node(token, parent->parent);
                if (node == NULL) {
                        error(token, "\"%.*s\" does not exist\n", token->length, token->pos);
                        return NULL;
                }

                lexer_next(token);
                if (token->kind == TK_LPAREN) {
                        lexer_next(token);
                        return parse_call(token, parent, node);
                } else if (node->kind == NK_VARIABLE) {
                        return parse_variable_reference(token, parent, node);
                } else {
                        error(token, "\"%.*s\" is not a variable\n", token->length, token->pos);
                        return NULL;
                }
        }

        if (token->kind == TK_NUMBER) {
                ast_node_t* number;

                number = create_node(parent);
                number->kind = NK_NUMBER;
                number->value = token->value;
                push_node(number);

                lexer_next(token);
                return number;
        }

        if (token->kind == TK_STRING) {
                ast_node_t* string;
                ast_node_t* reference;

                string = create_node(strings);
                string->kind = NK_STRING;
                string->string_id = next_string_id++;
                string->string_length = token->length - 1;
                string->string_data = token->pos + 1;
                push_node(string);

                reference = create_node(parent);
                reference->kind = NK_STRING_REFERENCE;
                reference->string = string;
                push_node(reference);

                lexer_next(token);
                return reference;
        }

        error(token, "Unexpected \"%.*s\"\n", token->length, token->pos);
        return NULL;
}

static void parse_arguments(token_t* token, ast_node_t* parent)
{
        DEBUG("Parsing arguments...\n");

        while (token->kind != TK_RPAREN) {
                ast_node_t* argument;

                argument = parse_value(token, parent);
                if (argument == NULL) {
                        return;
                }

                if (token->kind == TK_COMMA) {
                        lexer_next(token);
                        if (token->kind == TK_RPAREN) {
                                warn(token, "Extra \",\" after arguments\n");
                                break;
                        }
                }
        }

        lexer_next(token);
}

static ast_node_t* parse_call(token_t* token, ast_node_t* parent, ast_node_t* callee)
{
        ast_node_t* call;

        DEBUG("Parsing call to \"%.*s\"...\n", callee->name.length, callee->name.string);

        call = create_node(parent);
        call->kind = NK_CALL;
        call->callee = callee;

        parse_arguments(token, call);

        push_node(call);
        return call;
}

static ast_node_t* parse_variable_reference(token_t* token, ast_node_t* parent, ast_node_t* variable)
{
        ast_node_t* reference;

        DEBUG("Parsing variable reference to \"%.*s\"...\n", variable->name.length, variable->name.string);

        reference = create_node(parent);
        reference->kind = NK_VARIABLE_REFERENCE;
        reference->variable = variable;

        push_node(reference);
        return reference;
}

static ast_node_t* parse_assignment(token_t* token, ast_node_t* parent, ast_node_t* variable)
{
        ast_node_t* assignment;

        DEBUG("Parsing assignment...\n");

        assignment = create_node(parent);
        assignment->kind = NK_ASSIGNMENT;
        assignment->destination = variable;

        lexer_next(token);
        if (token->kind != TK_EQUALS || token->flags & TF_EQUALS) {
                error(token, "Variable name must be followed by a \"=\"\n");
                delete_node(assignment);
                return NULL;
        }

        lexer_next(token);
        if (parse_value(token, assignment) == NULL) {
                delete_node(assignment);
                return NULL;
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Assignment must be terminated with a \";\"\n");
                delete_node(assignment);
                return NULL;
        }

        push_node(assignment);
        lexer_next(token);
        return assignment;
}

static ast_node_t* parse_local_declaration(token_t* token, ast_node_t* procedure, ast_node_t* parent, ast_node_t* type)
{
        ast_node_t* variable;

        DEBUG("Parsing local declaration...\n");

        variable = create_node(parent);
        variable->kind = NK_VARIABLE;
        variable->flags = NF_NAMED;
        variable->type = type;
        variable->name.string = token->pos;
        variable->name.length = token->length;
        variable->name.hash = token->hash;
        variable->local_offset = procedure->local_size;
        procedure->local_size += type->size;

        /* Generate an assignment if the variable is initialized */
        lexer_next(token);
        if (token->kind == TK_EQUALS && !(token->flags & TF_EQUALS)) {
                ast_node_t* assignment;

                assignment = create_node(parent);
                assignment->kind = NK_ASSIGNMENT;
                assignment->destination = variable;

                lexer_next(token);
                if (parse_value(token, assignment) == NULL) {
                        delete_node(assignment);
                        delete_node(variable);
                        return NULL;
                }

                push_node(assignment);
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Variable declaration must be terminated with a \";\"\n");
                delete_node(variable);
                return NULL;
        }

        push_node(variable);
        lexer_next(token);
        return variable;
}

static void parse_parameters(token_t* token, ast_node_t* procedure)
{
        DEBUG("Parsing parameters..\n");

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

                /* Find parameter type */
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

                /* Commas seperate multiple parameters */
                lexer_next(token);
                if (token->kind == TK_COMMA) {
                        lexer_next(token);
                        if (token->kind == TK_RPAREN) {
                                warn(token, "Extra \",\" after parameters\n");
                                return;
                        }

                        continue;
                }

                if (token->kind != TK_RPAREN) {
                        error(token, "Parameter lists must be terminated with a \")\"\n");
                        return;
                }
        }
}

static void parse_procedure_body(token_t* token, ast_node_t* procedure)
{
        DEBUG("Parsing procedure body...\n");

        while (token->kind != TK_RCURLY) {
                ast_node_t* node;

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected identifier\n");
                        return;
                }

                node = find_node(token, procedure);
                if (node == NULL) {
                        error(token, "\"%.*s\" does not exist\n", token->length, token->pos);
                        return;
                }

                lexer_next(token);
                if (token->kind == TK_LPAREN) {
                        lexer_next(token);
                        parse_call(token, procedure, node);
                        if (token->kind != TK_SEMICOLON) {
                                error(token, "Call must be terminated with a \";\"\n");
                                return;
                        }

                        lexer_next(token);
                        DEBUG("Done with call...\n");
                } else if (token->kind == TK_EQUALS && !(token->flags & TF_EQUALS)) {
                        parse_assignment(token, procedure, node);
                } else if (token->kind == TK_IDENTIFIER) {
                        parse_local_declaration(token, procedure, procedure, node);
                } else {
                        error(token, "Expected \"(\", \"=\", or type after identifier\n", token->length, token->pos);
                        return;
                }
        }
}

static void parse_procedure(token_t* token)
{
        ast_node_t* procedure;

        DEBUG("Parsing procedure...\n");

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
