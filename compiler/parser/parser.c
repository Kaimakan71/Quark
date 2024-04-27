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
#include <parser/ast.h>
#include <parser/type.h>

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
        token_t token;

        DEBUG("parser: Parsing...");

        lexer_next(parser->lexer, &token);
        while (token.kind != TK_EOF) {
                error(&token, "Unexpected \"%.*s\"\n", token.length, token.pos);
                break;
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
