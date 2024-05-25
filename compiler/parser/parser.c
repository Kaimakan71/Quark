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

static ast_node_t* parse_public_declaration(parser_t* parser)
{
        ast_node_t* node;

        DEBUG("Parsing public declaration...");

        next_token(parser);
        if (parser->token.kind == TK_PROC) {
                node = parse_proc_declaration(parser);
        } else if (parser->token.kind == TK_TYPE) {
                node = parse_type_declaration(parser);
        } else {
                error(&parser->token, "Expected \"proc\" or \"type\" after \"public\"\n");
                return NULL;
        }

        if (node != NULL) {
                node->flags |= NF_PUBLIC;
        }

        return node;
}

void parser_destory(parser_t* parser)
{
        DEBUG("Destroying parser...");

        if (parser != NULL) {
                delete_nodes(parser->procedures);
                delete_nodes(parser->types);
                lexer_stream_destroy(parser->lexer_stream);
                free(parser);
        }
}

void parser_parse(parser_t* parser)
{
        DEBUG("Parsing...");

        next_token(parser);
        while (parser->token.kind != TK_EOF) {
                if (parser->token.kind == TK_PUBLIC) {
                        parse_public_declaration(parser);
                } else if (parser->token.kind == TK_PROC) {
                        parse_proc_declaration(parser);
                } else if (parser->token.kind == TK_TYPE) {
                        parse_type_declaration(parser);
                } else {
                        error(&parser->token, "Unexpected \"%.*s\"\n", parser->token.length, parser->token.pos);
                        break;
                }
        }
}

parser_t* create_parser(char* source)
{
        parser_t* parser;

        DEBUG("Creating parser...");

        parser = malloc(sizeof(parser_t));

        parser->lexer_stream = create_lexer_stream(source);
        if (parser->lexer_stream == NULL) {
                free(parser);
                return NULL;
        }

        parser->types = init_types();
        parser->procedures = create_node(NULL);

        return parser;
}
