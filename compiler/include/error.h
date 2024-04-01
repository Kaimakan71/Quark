/*
 * Error output functions.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _ERROR_H
#define _ERROR_H

#include <lexer/token.h>

void error(token_t* token, char* fmt, ...);
void warn(token_t* token, char* fmt, ...);

#endif /* _ERROR_H */
