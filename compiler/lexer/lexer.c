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

static void skip_whitespace(lexer_t* lexer)
{
        while (char_info[*lexer->pos] & CHAR_WHITESPACE) {
                if (char_info[*lexer->pos] & CHAR_VERT_WS) {
                        lexer->pos++;
                        lexer->line++;
                        lexer->line_start = lexer->pos;
                } else {
                        lexer->pos++;
                }
        }
}

static void lex_identifier(lexer_t* lexer, token_t* token)
{
        keyword_t* keyword;

        /* Find end of identifier */
        lexer->pos++;
        while (char_info[*lexer->pos] & CHAR_ALNUM || *lexer->pos == '_') {
                lexer->pos++;
        }

        /* Fill in token struct */
        token->kind = TK_IDENTIFIER;
        token->length = (size_t)(lexer->pos - token->pos);
        token->hash = hash_data(token->pos, token->length);

        /* Look for a keyword the identifier matches */
        keyword = find_keyword(token);
        if (keyword != NULL) {
                token->kind = keyword->value;
        }
}

static void lex_operator(lexer_t* lexer, token_t* token)
{
        token->length = 1;

        switch (*lexer->pos) {
        case '+':
                if (lexer->pos[1] == '+') {
                        token->kind = TK_INCREMENT;
                        token->length = 2;
                } else if (lexer->pos[1] == '=') {
                        token->kind = TK_PLUS;
                        token->flags |= TF_ASSIGNMENT;
                        token->length = 2;
                } else {
                        token->kind = TK_PLUS;
                }

                break;
        case '-':
                if (lexer->pos[1] == '>') {
                        token->kind = TK_ARROW;
                        token->length = 2;
                } else if (lexer->pos[1] == '-') {
                        token->kind = TK_DECREMENT;
                        token->length = 2;
                } else if (lexer->pos[1] == '=') {
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
                token->kind = char_info[*lexer->pos] >> CHAR_OPER_SHIFT;
                if (lexer->pos[1] == '=') {
                        token->flags |= TF_ASSIGNMENT;
                        token->length =  2;
                }

                break;
        }

        lexer->pos += token->length;
}

static void lex_number_base16(lexer_t* lexer, token_t* token)
{
        /* Calculate value of hex number */
        token->value = 0;
        lexer->pos += 2;
        while (char_info[*lexer->pos] & CHAR_HEX) {
                token->value <<= 4;

                if (char_info[*lexer->pos] == CHAR_XUPPER) {
                        token->value |= *lexer->pos++ - 'A' + 10;
                } else if (char_info[*lexer->pos] == CHAR_XLOWER) {
                        token->value |= *lexer->pos++ - 'a' + 10;
                } else {
                        token->value |= *lexer->pos++ - '0';
                }
        }
}

static void lex_number_base10(lexer_t* lexer, token_t* token)
{
        /* Calculate value of decimal number */
        token->value = *lexer->pos - '0';
        lexer->pos++;
        while (char_info[*lexer->pos] & CHAR_DIGIT) {
                token->value *= 10;
                token->value += *lexer->pos++ - '0';
        }
}

static void lex_string(lexer_t* lexer, token_t* token)
{
        /* TODO: Ignore escaped quotes */

        /* Find end of string */
        lexer->pos++;
        while (*lexer->pos != '"') {
                lexer->pos++;
        }
        lexer->pos++;

        token->kind = TK_STRING;
        token->length = (size_t)(lexer->pos - token->pos) - 1;
}

static void lex_character(lexer_t* lexer, token_t* token)
{
        /* TODO: Ignore escaped quotes */

        /* Find end of character */
        lexer->pos++;
        while (*lexer->pos != '\'') {
                lexer->pos++;
        }
        lexer->pos++;

        token->kind = TK_CHARACTER;
        token->length = (size_t)(lexer->pos - token->pos) - 1;
}

void lexer_destroy(lexer_t* lexer)
{
        DEBUG("Destroying lexer...");

        if (lexer != NULL) {
                free(lexer);
        }
}

void lexer_next(lexer_t* lexer, token_t* token)
{
        if (lexer == NULL || token == NULL) {
                return;
        }

        skip_whitespace(lexer);

        token->flags = TF_NONE;
        token->pos = lexer->pos;
        token->line = lexer->line;
        token->column = (int)(token->pos - lexer->line_start) + 1;

        if (char_info[*lexer->pos] & CHAR_ALPHA || *lexer->pos == '_') {
                lex_identifier(lexer, token);
                return;
        }

        if (char_info[*lexer->pos] & CHAR_SINGLE) {
                token->kind = char_info[*lexer->pos] >> CHAR_SINGLE_SHIFT;
                token->length = 1;
                lexer->pos++;
                return;
        }

        if (char_info[*lexer->pos] & CHAR_OPER) {
                lex_operator(lexer, token);
                return;
        }

        if (char_info[*lexer->pos] & CHAR_DIGIT) {
                token->kind = TK_NUMBER;

                if (lexer->pos[0] == '0' && lexer->pos[1] == 'x') {
                        lex_number_base16(lexer, token);
                } else {
                        lex_number_base10(lexer, token);
                }

                token->length = (size_t)(lexer->pos - token->pos);
                return;
        }

        if (*lexer->pos == '"') {
                lex_string(lexer, token);
                return;
        }

        if (*lexer->pos == '\'') {
                lex_character(lexer, token);
                return;
        }

        if (*lexer->pos == '\0') {
                token->kind = TK_EOF;
                return;
        }

        token->kind = TK_UNKNOWN;
        return;
}

lexer_t* create_lexer(char* source)
{
        lexer_t* lexer;

        DEBUG("Creating lexer...");

        if (source == NULL) {
                return NULL;
        }

        lexer = malloc(sizeof(lexer_t));
        lexer->pos = source;
        lexer->line_start = source;
        lexer->line = 1;

        /* TODO: Only initialize keywords once */
        init_keywords();

        return lexer;
}
