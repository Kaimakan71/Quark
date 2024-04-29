/*
 * Parses values.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_VALUE_H
#define _PARSER_VALUE_H

#include <parser.h>
#include <parser/ast.h>

ast_node_t* parse_value(parser_t* parser, ast_node_t* parent);

#endif /* !_PARSER_VALUE_H */
