/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <stdio.h>
#include <parser/ast.h>

void codegen(ast_node_t* root, ast_node_t* strings, FILE* out);

#endif /* _CODEGEN_H */
