/*
 * Groups characters into tokens.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include "quark/error.h"
#include "quark/char_info.h"
#include "quark/hash.h"
#include "quark/lexer.h"

static char* input;
static char* pos;
static char* line_start;
static int line;

static void skip_whitespace(void)
{
	while (char_info[*pos] & CHAR_WHITESPACE) {
		while (char_info[*pos] & CHAR_VERT_WS) {
			pos++;
			line++;
			line_start = pos;
		}

		while (char_info[*pos] & CHAR_HORZ_WS) {
			pos++;
		}
	}
}

static void lex_identifier(token_t* token)
{
	token->type = TT_IDENTIFIER;

	pos++;
	while (char_info[*pos] & CHAR_ALNUM) {
		pos++;
	}
	token->length = (size_t)(pos - token->pos);
	token->hash = hash_data(token->pos, token->length);
}

static void lex_number(token_t* token)
{
	token->type = TT_NUMBER;

	token->value = *pos - '0';
	pos++;
	while (char_info[*pos] & CHAR_DIGIT) {
		token->value *= 10;
		token->value += *pos++ - '0';
	}
	token->length = (size_t)(pos - token->pos);
}

static void lex_special(token_t* token)
{
	switch (*pos) {
	case '+':
		token->type = TT_PLUS;
		token->length = 1;
		pos++;
		break;
	case '-':
		token->type = TT_MINUS;
		token->length = 1;
		pos++;
		break;
	case '*':
		token->type = TT_STAR;
		token->length = 1;
		pos++;
		break;
	case '/':
		token->type = TT_SLASH;
		token->length = 1;
		pos++;
		break;
	}
}

void lexer_next(token_t* token)
{
	skip_whitespace();

	token->pos = pos;
	token->line = line;
	token->column = (int)(token->pos - line_start) + 1;

	if (char_info[*pos] & CHAR_ALPHA) {
		lex_identifier(token);
		return;
	}

	if (char_info[*pos] & CHAR_DIGIT) {
		lex_number(token);
		return;
	}

	if (char_info[*pos] & CHAR_SPECIAL) {
		lex_special(token);
		return;
	}

	if (*pos == '\0') {
		token->type = TT_EOF;
		return;
	}

	token->type = TT_UNKNOWN;
	error(token, "unexpected '%c'\n", *token->pos);
}

void lexer_init(char* source)
{
	input = source;
	pos = input;
	line_start = pos;
	line = 1;
}
