/*
 * Parses statements.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <string.h>
#include "log.h"
#include "parser/value.h"
#include "parser/statement.h"
#include "parser/variable.h"
#include "parser/procedure.h"

static ast_node_t* parse_return(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        ast_node_t* statement;

        debug("Parsing return...");

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

        debug("Parsing if...");

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

        debug("Parsing statement...");

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

        /* TODO: Structs will eventually have methods like console.print() */
        if (parser->token.kind == TK_LPAREN) {
                ast_node_t* call;

                call = parse_proc_call(parser, parent, &name);
                if (call == NULL) {
                        return NULL;
                }

                if (parser->token.kind != TK_SEMICOLON) {
                        error(&parser->token, "Expected \";\" after \")\"\n");
                        return NULL;
                }

                next_token(parser);
                return call;
        }

        return parse_local_declaration(parser, parent, procedure, &name);
}

bool parse_statement_group(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        debug("Parsing statement group...");

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
