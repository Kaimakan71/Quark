/*
 * Generates x86_64 assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_CODEGEN_H
#define _QUARK_CODEGEN_H

#include <stdio.h>
#include "quark/parser.h"

void codegen(ast_node_t* root_node, FILE* out);

#endif /* _QUARK_CODEGEN_H */
