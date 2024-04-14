/*
 * Keeps track of types.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _TYPES_H
#define _TYPES_H

#include <lexer/token.h>
#include <parser/ast.h>

ast_node_t* find_type(token_t* name);
void init_types();

#endif /* _TYPES_H */
