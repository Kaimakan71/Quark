/*
 * Parses statements.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <error.h>
#include <debug.h>
#include <parser/value.h>
#include <parser/statement.h>
#include <parser/storage.h>
#include <string.h>

static ast_node_t* parse_return(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        ast_node_t* statement;

        DEBUG("Parsing return...");

        statement = create_node(parent);
        statement->kind = NK_RETURN;
        statement->type = procedure->type;

        /* Allow returns with no value */
        if (next_token(parser)->kind == TK_SEMICOLON) {
                if (procedure->type != NULL) {
                        error(&parser->token, "Procedure \"%.*s\" must return a value\n", procedure->name.length, procedure->name.string);
                        delete_nodes(statement);
                        return NULL;
                }

                push_node(statement, NULL);
                return statement;
        }

        if (procedure->type == NULL) {
                error(&parser->token, "Procedure \"%.*s\" does not have a return type\n", procedure->name.length, procedure->name.string);
                delete_nodes(statement);
                return NULL;
        }

        /* Parse return value */
        if (parse_value(parser, statement) == NULL) {
                delete_nodes(statement);
                return NULL;
        }

        if (parser->token.kind != TK_SEMICOLON) {
                error(&parser->token, "Expected \";\" after return statement\n");
                delete_nodes(statement);
                return NULL;
        }

        push_node(statement, NULL);
        next_token(parser);
        return statement;
}

static ast_node_t* parse_if(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        ast_node_t* statement;
        ast_node_t* conditions;

        DEBUG("Parsing if...");

        if (next_token(parser)->kind != TK_LPAREN) {
                error(&parser->token, "Expected \"(\" after \"if\"\n");
                return NULL;
        }

	if (next_token(parser)->kind == TK_RPAREN) {
		error(&parser->token, "Expected conditions after \"(\"\n");
		return NULL;
	}

        statement = create_node(parent);
        statement->kind = NK_IF;

        conditions = create_node(statement);
        conditions->kind = NK_CONDITIONS;
        push_node(conditions, NULL);

        if (!parse_value(parser, conditions)) {
                delete_nodes(statement);
                return NULL;
        }

        if (parser->token.kind != TK_RPAREN) {
                error(&parser->token, "Expected \")\" after conditions\n");
                delete_nodes(statement);
                return NULL;
        }

        /* If body must start with a "{" */
        if (next_token(parser)->kind != TK_LCURLY) {
                error(&parser->token, "Expected \"{\" after \")\"\n");
                delete_nodes(statement);
                return NULL;
        }

        /* Parse body, if any */
        if (next_token(parser)->kind != TK_RCURLY) {
                if (!parse_statement_group(parser, statement, procedure)) {
                        delete_nodes(statement);
                        return NULL;
                }
        } else {
                next_token(parser);
        }

        push_node(statement, NULL);
        return statement;
}

ast_node_t* parse_statement(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
	token_t name;

        DEBUG("Parsing statement...");

        if (parser->token.kind == TK_RETURN) {
                return parse_return(parser, parent, procedure);
        } else if (parser->token.kind == TK_IF) {
                return parse_if(parser, parent, procedure);
        }

        if (parser->token.kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected statement\n");
                return NULL;
                
        }

        memcpy(&name, &parser->token, sizeof(token_t));
        next_token(parser);
        return parse_local_declaration(parser, parent, procedure, &name);
}

bool parse_statement_group(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        DEBUG("Parsing statement group...");

        while (parser->token.kind != TK_RCURLY) {
                ast_node_t* statement;

                statement = parse_statement(parser, parent, procedure);
                if (statement == NULL) {
                        return false;
                }
        }

        next_token(parser);
        return true;
}
