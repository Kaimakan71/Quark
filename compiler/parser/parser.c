/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <hash.h>
#include <lexer/lexer.h>
#include <parser/type.h>
#include <parser/parser.h>

static ast_node_t* procedures;
static ast_node_t* strings;
static int next_string_id;

static ast_node_t* parse_call(token_t* token, ast_node_t* parent, ast_node_t* callee);
static ast_node_t* parse_variable_reference(token_t* token, ast_node_t* parent, ast_node_t* variable);

static ast_node_t* parse_value(token_t* token, ast_node_t* parent)
{
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
                } else if (node->kind == NK_LOCAL_VARIABLE) {
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
                push_node(number, &parent->children);

                lexer_next(token);
                return number;
        }

        if (token->kind == TK_STRING) {
                ast_node_t* string;
                ast_node_t* reference;

                string = create_node(strings);
                string->kind = NK_STRING;
                string->id = next_string_id++;
                string->length = token->length - 1;
                string->data = token->pos + 1;
                push_node(string, &strings->children);

                reference = create_node(parent);
                reference->kind = NK_STRING_REFERENCE;
                reference->string = string;
                push_node(reference, &parent->children);

                lexer_next(token);
                return reference;
        }

        error(token, "Unexpected \"%.*s\"\n", token->length, token->pos);
        return NULL;
}

static ast_node_t* parse_variable_reference(token_t* token, ast_node_t* parent, ast_node_t* variable)
{
        ast_node_t* reference;

        reference = create_node(parent);
        reference->kind = NK_VARIABLE_REFERENCE;
        reference->variable = variable;

        push_node(reference, &parent->children);
        return reference;
}

static ast_node_t* parse_local_variable(token_t* token, ast_node_t* procedure, ast_node_t* parent, ast_node_t* type)
{
        ast_node_t* variable;

        variable = create_node(parent);
        variable->kind = NK_LOCAL_VARIABLE;
        variable->flags = NF_NAMED;
        variable->type = type;

        /* Find pointer depth */
        variable->pointer_depth = 0;
        lexer_next(token);
        while (token->kind == TK_STAR && !(token->flags & TF_EQUALS)) {
                variable->pointer_depth++;
                lexer_next(token);
        }

        /* Set variable name */
        if (token->kind != TK_IDENTIFIER) {
                error(token, "Expected variable name\n");
                delete_nodes(variable);
                return NULL;
        }
        variable->name.string = token->pos;
        variable->name.length = token->length;
        variable->name.hash = token->hash;

        /* Generate an assignment if the variable is initialized */
        lexer_next(token);
        if (token->kind == TK_EQUALS && !(token->flags & TF_EQUALS)) {
                ast_node_t* assignment;

                assignment = create_node(parent);
                assignment->kind = NK_ASSIGNMENT;
                assignment->destination = variable;

                /* TODO: Verify that value is the same type as variable */
                lexer_next(token);
                if (parse_value(token, assignment) == NULL) {
                        delete_nodes(assignment);
                        delete_nodes(variable);
                        return NULL;
                }

                push_node(assignment, &parent->children);
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Variable declaration must be terminated with a \";\"\n");
                delete_nodes(variable);
                return NULL;
        }

        /* Local size is different for pointers */
        variable->local_offset = procedure->local_size;
        if (variable->pointer_depth > 0) {
                procedure->local_size += sizeof(void*);
        } else {
                procedure->local_size += type->bytes;
        }

        push_node(variable, &parent->children);
        lexer_next(token);
        return variable;
}

static ast_node_t* parse_assignment(token_t* token, ast_node_t* parent, ast_node_t* variable)
{
        ast_node_t* assignment;

        assignment = create_node(parent);
        assignment->kind = NK_ASSIGNMENT;
        assignment->destination = variable;

        lexer_next(token);
        if (token->kind != TK_EQUALS || token->flags & TF_EQUALS) {
                error(token, "Variable name must be followed by a \"=\"\n");
                delete_nodes(assignment);
                return NULL;
        }

        /* TODO: Verify that value is the same type as destination */
        lexer_next(token);
        if (parse_value(token, assignment) == NULL) {
                delete_nodes(assignment);
                return NULL;
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Assignment must be terminated with a \";\"\n");
                delete_nodes(assignment);
                return NULL;
        }

        push_node(assignment, &parent->children);
        lexer_next(token);
        return assignment;
}

static void parse_arguments(token_t* token, ast_node_t* parent)
{
        int n_arguments;

        n_arguments = 0;
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

                n_arguments++;
        }

        if (n_arguments != parent->callee->n_parameters) {
                error(token, "\"%.*s\" requires %d arguments\n", parent->callee->name.length, parent->callee->name.string, parent->callee->n_parameters);
        }

        lexer_next(token);
}

static ast_node_t* parse_call(token_t* token, ast_node_t* parent, ast_node_t* callee)
{
        ast_node_t* call;

        call = create_node(parent);
        call->kind = NK_CALL;
        call->callee = callee;

        parse_arguments(token, call);

        push_node(call, &parent->children);
        return call;
}

static ast_node_t* parse_return(token_t* token, ast_node_t* parent)
{
        ast_node_t* ret;

        ret = create_node(parent);
        ret->kind = NK_RETURN;

        /* Allow returning with no value */
        lexer_next(token);
        if (token->kind == TK_SEMICOLON) {
                lexer_next(token);
                return ret;
        }

        if (parse_value(token, ret) == NULL) {
                delete_nodes(ret);
                return NULL;
        }

        if (token->kind != TK_SEMICOLON) {
                error(token, "Return must be terminated with a \";\"\n");
                delete_nodes(ret);
                return NULL;
        }

        push_node(ret, &parent->children);
        lexer_next(token);
        return ret;
}

static void parse_parameters(token_t* token, ast_node_t* procedure)
{
        while (token->kind != TK_RPAREN) {
                ast_node_t* parameter;

                parameter = create_node(procedure);
                parameter->kind = NK_PARAMETER;
                parameter->flags |= NF_NAMED;

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected parameter type\n");
                        delete_nodes(parameter);
                        return;
                }

                /* Find parameter type */
                parameter->type = find_type(token);
                if (parameter->type == NULL) {
                        error(token, "type \"%.*s\" does not exist\n", token->length, token->pos);
                        delete_nodes(parameter);
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
                        delete_nodes(parameter);
                        return;
                }

                /* Prevent redeclarations */
                if (find_node(token, procedure) != NULL) {
                        error(token, "\"%.*s\" has already been declared\n", token->length, token->pos);
                        delete_nodes(parameter);
                        return;
                }

                parameter->name.string = token->pos;
                parameter->name.length = token->length;
                parameter->name.hash = token->hash;
                push_node(parameter, &procedure->parameters);
                procedure->n_parameters++;

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
        while (token->kind != TK_RCURLY) {
                ast_node_t* node;

                if (token->kind == TK_RETURN) {
                        parse_return(token, procedure);
                        return;
                }

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected identifier\n");
                        return;
                }

                node = find_type(token);
                if (node != NULL) {
                        parse_local_variable(token, procedure, procedure, node);
                        continue;
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
                } else if (token->kind == TK_EQUALS && !(token->flags & TF_EQUALS)) {
                        parse_assignment(token, procedure, node);
                } else {
                        error(token, "Expected \"(\", \"=\", or type after identifier\n", token->length, token->pos);
                        return;
                }
        }
}

static void parse_procedure(token_t* token)
{
        ast_node_t* procedure;

        procedure = create_node(procedures);
        procedure->kind = NK_PROCEDURE;
        procedure->flags = NF_NAMED;
        procedure->n_parameters = 0;
        procedure->local_size = 0;

        /* Procedure name */
        lexer_next(token);
        if (token->kind != TK_IDENTIFIER) {
                error(token, "\"proc\" must be followed by the procedure's name\n");
                delete_nodes(procedure);
                return;
        }
        procedure->name.string = token->pos;
        procedure->name.length = token->length;
        procedure->name.hash = token->hash;

        lexer_next(token);
        if (token->kind != TK_LPAREN) {
                error(token, "Procedure name must be followed by a \"(\"\n");
                delete_nodes(procedure);
                return;
        }

        /* Procedure parameters */
        lexer_next(token);
        parse_parameters(token, procedure);
        if (token->kind != TK_RPAREN) {
                error(token, "Parameter list must be followed by a \")\"\n");
                delete_nodes(procedure);
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

                procedure->type = find_type(token);
                if (procedure->type == NULL) {
                        error(token, "type \"%.*s\" does not exist\n", token->length, token->pos);
                        return;
                }

                lexer_next(token);
        } else {
                procedure->type = NULL;
        }

        /* Procedure declarations may be terminated by a ";" */
        if (token->kind == TK_SEMICOLON) {
                push_node(procedure, &procedures->children);
                lexer_next(token);
                return;
        }

        /* If there is no semicolon, a "{" must start the procedure body */
        if (token->kind != TK_LCURLY) {
                error(token, "Procedure declaration must be terminated with a \";\" or \"{\"\n");
                delete_nodes(procedure);
                return;
        }

        /* Parse procedure body, if not empty */
        lexer_next(token);
        if (token->kind != TK_RCURLY) {
                parse_procedure_body(token, procedure);
                if (token->kind != TK_RCURLY) {
                        delete_nodes(procedure);
                        return;
                }
        }

        push_node(procedure, &procedures->children);
        lexer_next(token);
}

ast_node_t* parse(char* source, ast_node_t** strings_out)
{
        token_t token;

        procedures = create_node(NULL);
        procedures->kind = NK_UNKNOWN;

        strings = create_node(NULL);
        strings->kind = NK_UNKNOWN;
        next_string_id = 0;

        init_types();

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
        return procedures;
}
