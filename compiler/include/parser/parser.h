/*
 * Analyzes tokens and generates symbols.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_H
#define _PARSER_H

#include <parser/ast.h>

ast_node_t* parse(char* source, ast_node_t** strings);

#endif /* _PARSER_H */
