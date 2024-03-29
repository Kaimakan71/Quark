/*
 * Error output functions.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdarg.h>
#include <stdio.h>
#include <error.h>

void error(token_t* token, char* fmt, ...)
{
        va_list ap;

        fprintf(stderr, "%d:%d: error: ", token->line, token->column);

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
}
