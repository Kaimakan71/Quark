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
} lexer_stream_t;

void lexer_stream_destroy(lexer_stream_t* stream);
void lexer_stream_next(lexer_stream_t* stream, token_t* token);
lexer_stream_t* create_lexer_stream(char* source);

#endif /* !_LEXER_H */
