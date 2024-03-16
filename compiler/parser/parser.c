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
#include <parser/parser.h>

static ast_node_t* root;
static ast_node_t* strings;
static unsigned int next_string_id;

static void create_builtin_type(char* name, size_t bits, uint8_t flags)
{
        ast_node_t* type;

        type = create_node(root);
        type->kind = NK_BUILTIN_TYPE;
        type->flags |= flags | NF_NAMED;
        type->name.string = name;
        type->name.length = strlen(name);
        type->name.hash = hash_data(name, type->name.length);
        type->bits = bits;

        push_node(type);
}

static ast_node_t* find_type(token_t* name)
{
	ast_node_t* type;

	type = find_node(name, root);
	if (type == NULL) {
                error(name, "\"%.*s\" was not found in any accessible scope\n", name->length, name->pos);
                return NULL;
	}

	if (type->kind != NK_BUILTIN_TYPE) {
		error(name, "%.*s is not a type\n", name->length, name->pos);
		return NULL;
	}

        return type;
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
                        delete_node(parameter);
                        return;
                }

                parameter->type = find_type(token);
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
                        error(token, "Expected parameter name after type\n");
                        delete_node(parameter);
                        return;
                }

                /* Prevent redeclarations */
                if (find_node(token, root) != NULL) {
                        error(token, "\"%.*s\" has already been declared\n", token->length, token->pos);
                        delete_node(parameter);
                        return;
                }

                parameter->name.string = token->pos;
                parameter->name.length = token->length;
                parameter->name.hash = token->hash;

		push_node(parameter);
		lexer_next(token);
		if (token->kind == TK_COMMA) {
			lexer_next(token);
		}
        }
}

static void parse_arguments(token_t* token, ast_node_t* call)
{
	while (token->kind != TK_RPAREN) {
		ast_node_t* argument;
                ast_node_t* string;

		argument = create_node(call);

		if (token->kind == TK_NUMBER) {
                        argument->kind = NK_NUMBER;
                        argument->value = token->value;
		} else if (token->kind == TK_STRING) {
                        string = create_node(strings);
                        string->kind = NK_STRING;
                        string->string_id = next_string_id++;
                        string->string_length = token->length - 1;
                        string->string_data = token->pos + 1;
                        push_node(string);

                        argument->kind = NK_STRING_REFERENCE;
                        argument->string = string;
                } else {
			error(token, "Expected value or \")\"\n");
                        delete_node(argument);
			return;
                }

		push_node(argument);
		lexer_next(token);
		if (token->kind == TK_COMMA) {
			lexer_next(token);
		}
	}
}

static void parse_procedure_body(token_t* token, ast_node_t* procedure)
{
        while (token->kind != TK_RCURLY) {
                ast_node_t* call;

                call = create_node(procedure);
                call->kind = NK_CALL;

                if (token->kind != TK_IDENTIFIER) {
                        error(token, "Expected procedure name\n");
                        delete_node(call);
                        return;
                }

                call->callee = find_node_of_kind(token, procedure, NK_PROCEDURE);
                if (call->callee == NULL) {
                        delete_node(call);
                        return;
                }

                lexer_next(token);
                if (token->kind != TK_LPAREN) {
                        error(token, "Expected \"(\" after procedure name\n");
                        delete_node(call);
                        return;
                }

		lexer_next(token);
		parse_arguments(token, call);

                if (token->kind != TK_RPAREN) {
                        error(token, "Expected \")\" after procedure arguments\n");
                        delete_node(call);
                        return;
                }

                lexer_next(token);
                if (token->kind != TK_SEMICOLON) {
                        error(token, "Expected \";\" after \")\"\n");
                        delete_node(call);
                        return;
                }

                push_node(call);
                lexer_next(token);
        }
}

static void parse_procedure(token_t* token)
{
        ast_node_t* procedure;

        procedure = create_node(root);
        procedure->kind = NK_PROCEDURE;
        procedure->flags = NF_NAMED;

        /* Procedure name */
        lexer_next(token);
        if (token->kind != TK_IDENTIFIER) {
                error(token, "Expected procedure name after \"proc\"\n");
                delete_node(procedure);
                return;
        }
        procedure->name.string = token->pos;
        procedure->name.length = token->length;
        procedure->name.hash = token->hash;

        lexer_next(token);
        if (token->kind != TK_LPAREN) {
                error(token, "Expected \"(\" after procedure name\n");
                delete_node(procedure);
                return;
        }

        /* Procedure parameters */
        lexer_next(token);
        parse_parameters(token, procedure);
        if (token->kind != TK_RPAREN) {
                delete_node(procedure);
                return;
        }

        /* Procedure declarations may be terminated by a ";" */
        lexer_next(token);
        if (token->kind == TK_SEMICOLON) {
                push_node(procedure);
                lexer_next(token);
                return;
        }

        /* If there is no semicolon, a "{" must start the procedure body */
        if (token->kind != TK_LCURLY) {
                error(token, "Expected \";\" or \"{\" after \")\"\n");
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

ast_node_t* parse(char* source, ast_node_t** _strings)
{
        token_t token;

        root = malloc(sizeof(ast_node_t));
        strings = malloc(sizeof(ast_node_t));
        memset(root, 0, sizeof(ast_node_t));
        memset(strings, 0, sizeof(ast_node_t));

        next_string_id = 0;

        create_builtin_type("uint", 64, NF_NONE);
        create_builtin_type("char", 8, NF_CHARACTER);

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

        *_strings = strings;
        return root;
}
