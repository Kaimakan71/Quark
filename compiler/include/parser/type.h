/*
 * Keeps track of types.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_TYPE_H
#define _PARSER_TYPE_H

#include <parser.h>
#include <lexer/token.h>
#include <parser/ast.h>

ast_node_t* parse_type_reference(parser_t* parser, ast_node_t* node, token_t* type_name);
ast_node_t* parse_type_declaration(parser_t* parser);
ast_node_t* init_types(void);

#endif /* !_PARSER_TYPE_H */
