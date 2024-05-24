/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <hash.h>
#include <lexer.h>
#include <lexer/char_info.h>
#include <lexer/keyword.h>

static void skip_whitespace(lexer_stream_t* stream)
{
        while (char_info[*stream->pos] & CHAR_WHITESPACE) {
                if (char_info[*stream->pos] & CHAR_VERT_WS) {
                        stream->pos++;
                        stream->line++;
                        stream->line_start = stream->pos;
                } else {
                        stream->pos++;
                }
        }
}

static void lex_identifier(lexer_stream_t* stream, token_t* token)
{
        keyword_t* keyword;

        /* Find end of identifier */
        stream->pos++;
        while (char_info[*stream->pos] & CHAR_ALNUM || *stream->pos == '_') {
                stream->pos++;
        }

        /* Fill in token struct */
        token->kind = TK_IDENTIFIER;
        token->length = (size_t)(stream->pos - token->pos);
        token->hash = hash_data(token->pos, token->length);

        /* Look for a keyword the identifier matches */
        keyword = find_keyword(token);
        if (keyword != NULL) {
                token->kind = keyword->value;
        }
}

static void lex_operator(lexer_stream_t* stream, token_t* token)
{
        token->length = 1;

        switch (*stream->pos) {
        case '+':
                if (stream->pos[1] == '+') {
                        token->kind = TK_INCREMENT;
                        token->length = 2;
                } else if (stream->pos[1] == '=') {
                        token->kind = TK_PLUS;
                        token->flags |= TF_ASSIGNMENT;
                        token->length = 2;
                } else {
                        token->kind = TK_PLUS;
                }

                break;
        case '-':
                if (stream->pos[1] == '>') {
                        token->kind = TK_ARROW;
                        token->length = 2;
                } else if (stream->pos[1] == '-') {
                        token->kind = TK_DECREMENT;
                        token->length = 2;
                } else if (stream->pos[1] == '=') {
                        token->kind = TK_MINUS;
                        token->flags |= TF_ASSIGNMENT;
                        token->length = 2;
                } else {
                        token->kind = TK_MINUS;
                }

                break;
        case '~':
                token->kind = TK_TILDE;
                break;
        case '=':
                token->kind = TK_EQUALS;
                break;
        default:
                token->kind = char_info[*stream->pos] >> CHAR_OPER_SHIFT;
                if (stream->pos[1] == '=') {
                        token->flags |= TF_ASSIGNMENT;
                        token->length =  2;
                }

                break;
        }

        stream->pos += token->length;
}

static void lex_number_base16(lexer_stream_t* stream, token_t* token)
{
        /* Calculate value of hex number */
        token->value = 0;
        stream->pos += 2;
        while (char_info[*stream->pos] & CHAR_HEX) {
                token->value <<= 4;

                if (char_info[*stream->pos] == CHAR_XUPPER) {
                        token->value |= *stream->pos++ - 'A' + 10;
                } else if (char_info[*stream->pos] == CHAR_XLOWER) {
                        token->value |= *stream->pos++ - 'a' + 10;
                } else {
                        token->value |= *stream->pos++ - '0';
                }
        }
}

static void lex_number_base10(lexer_stream_t* stream, token_t* token)
{
        /* Calculate value of decimal number */
        token->value = *stream->pos - '0';
        stream->pos++;
        while (char_info[*stream->pos] & CHAR_DIGIT) {
                token->value *= 10;
                token->value += *stream->pos++ - '0';
        }
}

static void lex_string(lexer_stream_t* stream, token_t* token)
{
        /* Find end of string */
        stream->pos++;
        while (*stream->pos != '"') {
		if (stream->pos[0] == '\\' && stream->pos[1] == '\"') {
			stream->pos++;
		}

                stream->pos++;
        }
        stream->pos++;

        token->kind = TK_STRING;
        token->length = (size_t)(stream->pos - token->pos) - 1;
}

static void lex_character(lexer_stream_t* stream, token_t* token)
{
        /* Find end of character */
        stream->pos++;
        while (*stream->pos != '\'') {
		if (stream->pos[0] == '\\' && stream->pos[1] == '\'') {
			stream->pos++;
		}

                stream->pos++;
        }
        stream->pos++;

        token->kind = TK_CHARACTER;
        token->length = (size_t)(stream->pos - token->pos) - 1;
}

void lexer_stream_destroy(lexer_stream_t* stream)
{
        DEBUG("Destroying lexer stream...");

        if (stream != NULL) {
                free(stream);
        }
}

void lexer_stream_next(lexer_stream_t* stream, token_t* token)
{
        if (stream == NULL || token == NULL) {
                return;
        }

        skip_whitespace(stream);

        token->flags = TF_NONE;
        token->pos = stream->pos;
        token->line = stream->line;
        token->column = (int)(token->pos - stream->line_start) + 1;

        if (char_info[*stream->pos] & CHAR_ALPHA || *stream->pos == '_') {
                lex_identifier(stream, token);
                return;
        }

        if (char_info[*stream->pos] & CHAR_SINGLE) {
                token->kind = char_info[*stream->pos] >> CHAR_SINGLE_SHIFT;
                token->length = 1;
                stream->pos++;
                return;
        }

        if (char_info[*stream->pos] & CHAR_OPER) {
                lex_operator(stream, token);
                return;
        }

        if (char_info[*stream->pos] & CHAR_DIGIT) {
                token->kind = TK_NUMBER;

                if (stream->pos[0] == '0' && stream->pos[1] == 'x') {
                        lex_number_base16(stream, token);
                } else {
                        lex_number_base10(stream, token);
                }

                token->length = (size_t)(stream->pos - token->pos);
                return;
        }

        if (*stream->pos == '"') {
                lex_string(stream, token);
                return;
        }

        if (*stream->pos == '\'') {
                lex_character(stream, token);
                return;
        }

        if (*stream->pos == '\0') {
                token->kind = TK_EOF;
                return;
        }

        token->kind = TK_UNKNOWN;
        return;
}

lexer_stream_t* create_lexer_stream(char* source)
{
        lexer_stream_t* stream;

        DEBUG("Creating lexer stream...");

        if (source == NULL) {
                return NULL;
        }

        stream = malloc(sizeof(lexer_stream_t));
        stream->pos = source;
        stream->line_start = source;
        stream->line = 1;

        /* TODO: Only initialize keywords once */
        init_keywords();

        return stream;
}
