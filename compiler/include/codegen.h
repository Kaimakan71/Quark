/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <stdbool.h>
#include <stdio.h>
#include <parser.h>

bool codegen(parser_t* parser, FILE* out);

#endif /* !_CODEGEN_H */
