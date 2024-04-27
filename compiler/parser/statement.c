/*
 * Parses statements.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <error.h>
#include <debug.h>
#include <parser.h>
#include <parser/statement.h>

static ast_node_t* parse_return(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        ast_node_t* ret;

        DEBUG("parser: Parsing return...");

        ret = create_node(parent);
        ret->kind = NK_RETURN;

        /* Allow returns with no value */
        if (next_token(parser)->kind == TK_SEMICOLON) {
                if (procedure->type != NULL) {
                        error(&parser->token, "Procedure \"%.*s\" must return a value\n", procedure->name.length, procedure->name.string);
                        delete_nodes(ret);
                        return NULL;
                }

                push_node(ret, NULL);
                return ret;
        }

        if (procedure->type == NULL) {
                error(&parser->token, "Procedure \"%.*s\" does not have a return type\n", procedure->name.length, procedure->name.string);
                delete_nodes(ret);
                return NULL;
        }

        /* TODO: Support more kinds of values */
        if (parser->token.kind != TK_NUMBER) {
                error(&parser->token, "Expected number\n");
                delete_nodes(ret);
                return NULL;
        }
        ast_node_t* num = create_node(ret);
        num->kind = NK_NUMBER;
        num->value = parser->token.value;
        push_node(num, NULL);
        /* X */

        push_node(ret, NULL);
        next_token(parser);
        return ret;
}

ast_node_t* parse_statement(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        DEBUG("parser: Parsing statement...");

        /* TODO: Support other kinds of statements */
        if (parser->token.kind != TK_RETURN) {
                error(&parser->token, "Expected \"return\"\n");
                return NULL;
        }

        return parse_return(parser, parent, procedure);
}

bool parse_statement_group(parser_t* parser, ast_node_t* parent, ast_node_t* procedure)
{
        DEBUG("parser: Parsing statement group...");

        while (parser->token.kind != TK_RCURLY) {
                ast_node_t* statement;

                statement = parse_statement(parser, parent, procedure);
                if (statement == NULL) {
                        return false;
                }

                /* Statements are seperated by ";" */
                if (parser->token.kind != TK_SEMICOLON) {
                        error(&parser->token, "Expected \";\" after statement\n");
                        return false;
                }

                next_token(parser);
        }

        return true;
}
