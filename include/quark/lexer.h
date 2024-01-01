/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_LEXER_H
#define _QUARK_LEXER_H

#include <stdint.h>

typedef enum {
	TT_UNKNOWN,
	TT_EOF,

	TT_IDENTIFIER,
	TT_NUMBER,

	TT_SEMICOLON,
	TT_EQUALS,
	TT_PLUS,
	TT_MINUS,
	TT_STAR,
	TT_SLASH,

	TT_UINT
} token_type_t;

typedef struct {
	token_type_t type;

	char* pos;
	size_t length;
	int line;
	int column;

	union {
		uint32_t hash;
		uint64_t value;
	};
} token_t;

void lexer_next(token_t* token);
void lexer_init(char* source);

#endif /* _QUARK_LEXER_H */
