/*
 * Lexer token definitions.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _LEXER_TOKEN_H
#define _LEXER_TOKEN_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
        TK_UNKNOWN,
        TK_EOF,

        TK_IDENTIFIER,
        TK_STRING,
        TK_NUMBER,
        TK_CHARACTER,

        /* Seperators */
        TK_COMMA,
        TK_DOT,
        TK_COLON,
        TK_SEMICOLON,
        TK_LPAREN,
        TK_RPAREN,
        TK_LCURLY,
        TK_RCURLY,
        TK_LSQUARE,
        TK_RSQUARE,

        /* Operators */
        TK_ARROW,
        TK_INCREMENT,
        TK_DECREMENT,
        TK_PLUS,
        TK_MINUS,
        TK_STAR,
        TK_SLASH,
        TK_PERCENT,
        TK_EQUALS,
        TK_EXCLAMATION,
        TK_LESS_THAN,
        TK_GREATER_THAN,
        TK_CARET,
        TK_AMPERSAND,
        TK_PIPE,
        TK_TILDE,

        /* Keywords */
        TK_PROC,
        TK_PUBLIC,
        TK_RETURN
} token_kind_t;

#define TF_NONE 0
#define TF_ASSIGNMENT (1 << 0)

typedef struct {
        token_kind_t kind;
        uint8_t flags;

        char* pos;
        int line;
        int column;
        size_t length;

        union {
                uint32_t hash;
                uint64_t value;
        };
} token_t;

#endif /* !_LEXER_TOKEN_H */
