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

typedef struct {
        parser_t* parser;
        size_t bytes;
        FILE* out;
} codegen_t;

bool codegen(codegen_t* generator);

#endif /* !_CODEGEN_H */
