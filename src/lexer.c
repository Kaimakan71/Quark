/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <string.h>
#include "quark/error.h"
#include "quark/char_info.h"
#include "quark/hash.h"
#include "quark/lexer.h"

typedef struct keyword {
	char* string;
	size_t length;
	uint32_t hash;

	token_type_t token_type;

	struct keyword* next;
} keyword_t;

static char* input;
static char* pos;
static char* line_start;
static int line;
static keyword_t* keywords_head;
static keyword_t* keywords_tail;

static void add_keyword(char* string, token_type_t token_type)
{
	keyword_t* keyword;

	keyword = malloc(sizeof(keyword_t));
	keyword->string = string;
	keyword->length = strlen(string);
	keyword->hash = hash_data(keyword->string, keyword->length);
	keyword->token_type = token_type;
	keyword->next = NULL;

	if (keywords_head == NULL) {
		keywords_head = keyword;
	} else {
		keywords_tail->next = keyword;
	}
	keywords_tail = keyword;
}

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

	/* Find identifier length */
	pos++;
	while (char_info[*pos] & CHAR_ALNUM) {
		pos++;
	}
	token->length = (size_t)(pos - token->pos);

	/* Find the keyword associated with this identifier */
	token->hash = hash_data(token->pos, token->length);
	for (keyword_t* keyword = keywords_head; keyword != NULL; keyword = keyword->next) {
		if (token->hash == keyword->hash && token->length == keyword->length) {
			token->type = keyword->token_type;
			break;
		}
	}
}

static void lex_number(token_t* token)
{
	token->type = TT_NUMBER;

	/* Calculate value of number */
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
	/* Special cases for operators (like +, +=, ++) */
	switch (*pos) {
	case '=':
		token->type = TT_EQUALS;
		token->length = 1;
		break;
	case '+':
		token->type = TT_PLUS;
		token->length = 1;
		break;
	case '-':
		token->type = TT_MINUS;
		token->length = 1;
		break;
	case '*':
		token->type = TT_STAR;
		token->length = 1;
		break;
	case '/':
		token->type = TT_SLASH;
		token->length = 1;
		break;
	}

	pos += token->length;
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

	if (char_info[*pos] & CHAR_SINGLE) {
		token->type = char_info[*pos] >> 8;
		token->length = 1;
		pos++;
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

	add_keyword("uint", TT_UINT);
}
