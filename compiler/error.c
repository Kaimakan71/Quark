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

        fprintf(stderr, "%d:%d: \033[91merror\033[0m: ", token->line, token->column);

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
}

void warn(token_t* token, char* fmt, ...)
{
	va_list ap;

	printf("%d:%d: \033[93mwarning\033[0m: ", token->line, token->column);

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

