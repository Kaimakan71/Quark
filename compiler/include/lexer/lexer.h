/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _LEXER_H
#define _LEXER_H

#include <lexer/token.h>

void lexer_next(token_t* token);
void lexer_init(char* source);

#endif /* _LEXER_H */
