/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _LEXER_H
#define _LEXER_H

#include <stdbool.h>
#include <lexer/token.h>

typedef struct {
        char* pos;
        char* line_start;
        int line;
} lexer_t;

void lexer_destroy(lexer_t* lexer);
void lexer_next(lexer_t* lexer, token_t* token);
lexer_t* create_lexer(char* source);

#endif /* !_LEXER_H */
