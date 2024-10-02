/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"
#include "parser/ast.h"

typedef struct {
        lexer_t lexer;
        token_t token;
        ast_node_t* types;
        ast_node_t* procedures;
} parser_t;

static inline token_t* next_token(parser_t* parser)
{
        lexer_next(&parser->lexer, &parser->token);
        return &parser->token;
}

void parser_destory(parser_t* parser);
void parser_parse(parser_t* parser);
void parser_init(parser_t* parser, char* source);

#endif /* !_PARSER_H */
