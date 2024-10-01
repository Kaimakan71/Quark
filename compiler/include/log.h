/*
 * Logging output functions.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _LOG_H
#define _LOG_H

#include "lexer/token.h"

#ifdef ENABLE_DEBUG
void __debug(const char* func, const char* msg);

#define debug(msg) __debug(__func__, msg)
#else
#define debug(msg)
#endif

void error(token_t* token, const char* fmt, ...);
void warn(token_t* token, const char* fmt, ...);

#endif /* !_LOG_H */
