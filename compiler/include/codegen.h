/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <stdbool.h>
#include <stdio.h>
#include "parser/ast.h"

bool codegen(ast_node_t* procedures, FILE* fp, size_t word_bytes);

#endif /* !_CODEGEN_H */
