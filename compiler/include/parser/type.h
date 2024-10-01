/*
 * Keeps track of types.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _PARSER_TYPE_H
#define _PARSER_TYPE_H

#include "lexer/token.h"
#include "parser.h"

ast_node_t* parse_type_reference(parser_t* parser, ast_node_t* node, token_t* type_name);
ast_node_t* parse_type_declaration(parser_t* parser);
ast_node_t* init_types(void);

#endif /* !_PARSER_TYPE_H */
