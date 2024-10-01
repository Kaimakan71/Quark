/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <stdbool.h>
#include <stdlib.h>
#include "lexer.h"
#include "log.h"
#include "parser.h"
#include "parser/type.h"
#include "parser/procedure.h"

void parser_destory(parser_t* parser)
{
        debug("Destroying parser...");

        if (parser != NULL) {
                delete_nodes(parser->procedures);
                delete_nodes(parser->types);
                free(parser);
        }
}

void parser_parse(parser_t* parser)
{
        debug("Parsing...");

        next_token(parser);
        while (parser->token.kind != TK_EOF) {
                ast_node_t* node;
                bool public = false;

                if (parser->token.kind == TK_PUB) {
                        public = true;
                        next_token(parser);
                }

                if (parser->token.kind == TK_PROC) {
                        node = parse_proc_declaration(parser);
                } else if (parser->token.kind == TK_TYPE) {
                        node = parse_type_declaration(parser);
                } else {
                        error(&parser->token, "Unexpected \"%.*s\"\n", parser->token.length, parser->token.pos);
                        break;
                }

                if (public && node != NULL) {
                        node->flags |= NF_PUBLIC;
                }
        }
}

parser_t* create_parser(char* source)
{
        parser_t* parser;

        debug("Creating parser...");

        parser = malloc(sizeof(parser_t));

        lexer_init(&parser->lexer, source);
        parser->types = init_types();
        parser->procedures = create_node(NULL);

        return parser;
}
