/*
 * Logging output functions.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <stdarg.h>
#include <stdio.h>
#include "log.h"

void __debug(const char* func, const char* msg)
{
        printf("%s(): \033[90mdebug\033[0m: %s\n", func, msg);
}

void error(token_t* token, const char* fmt, ...)
{
        va_list ap;

        fprintf(stderr, "%d:%d: \033[91merror\033[0m: ", token->line, token->column);

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
}

void warn(token_t* token, const char* fmt, ...)
{
        va_list ap;

        printf("%d:%d: \033[93mwarning\033[0m: ", token->line, token->column);

        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
}
