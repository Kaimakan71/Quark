/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <debug.h>
#include <error.h>
#include <lexer.h>
#include <parser.h>
#include <parser/type.h>
#include <parser/procedure.h>

static ast_node_t* parse_public(parser_t* parser)
{
        DEBUG("parser: Parsing public declaration...");

        next_token(parser);

        if (parser->token.kind == TK_PROC) {
                return parse_procedure(parser, true);
        }

        error(&parser->token, "Expected \"proc\" after \"public\"\n");
        return NULL;
}

void parser_destory(parser_t* parser)
{
        DEBUG("parser: Destroying parser...");

        if (parser != NULL) {
                delete_nodes(parser->procedures);
                delete_nodes(parser->types);
                lexer_destroy(parser->lexer);
                free(parser);
        }
}

void parser_parse(parser_t* parser)
{
        DEBUG("parser: Parsing...");

        next_token(parser);
        while (parser->token.kind != TK_EOF) {
                if (parser->token.kind == TK_PUBLIC) {
                        parse_public(parser);
                } else if (parser->token.kind == TK_PROC) {
                        parse_procedure(parser, false);
                } else {
                        error(&parser->token, "Unexpected \"%.*s\"\n", parser->token.length, parser->token.pos);
                        break;
                }
        }
}

parser_t* create_parser(char* source)
{
        parser_t* parser;

        DEBUG("parser: Creating parser...");

        parser = malloc(sizeof(parser_t));

        parser->lexer = create_lexer(source);
        if (parser->lexer == NULL) {
                free(parser);
                return NULL;
        }

        parser->types = init_types();
        parser->procedures = create_node(NULL);

        return parser;
}
