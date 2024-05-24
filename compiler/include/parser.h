/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_H
#define _PARSER_H

#include <lexer.h>
#include <parser/ast.h>

typedef struct {
        lexer_stream_t* lexer_stream;
        token_t token;
        ast_node_t* types;
        ast_node_t* procedures;
} parser_t;

static inline token_t* next_token(parser_t* parser)
{
        lexer_stream_next(parser->lexer_stream, &parser->token);
        return &parser->token;
}

void parser_destory(parser_t* parser);
void parser_parse(parser_t* parser);
parser_t* create_parser(char* source);

#endif /* !_PARSER_H */
