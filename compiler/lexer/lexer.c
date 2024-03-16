/*
 * Groups characters into tokens.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <string.h>
#include <error.h>
#include <hash.h>
#include <lexer/char_info.h>
#include <lexer/keyword.h>
#include <lexer/lexer.h>

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
        keyword_t* keyword;

        /* Find end of identifier */
        pos++;
        while (char_info[*pos] & CHAR_ALNUM || *pos == '_') {
                pos++;
        }

        /* Fill in token struct */
        token->kind = TK_IDENTIFIER;
        token->length = (size_t)(pos - token->pos);
        token->hash = hash_data(token->pos, token->length);

        /* Look for a keyword the identifier matches */
        keyword = find_keyword(token);
        if (keyword != NULL) {
                token->kind = keyword->value;
        }
}

static void lex_operator(token_t* token)
{
        token->length = 1;

        if (*pos == '+') {
                if (pos[1] == '+') {
                        token->kind = TK_INCREMENT;
                        token->length = 2;
                } else if (pos[1] == '=') {
                        token->kind = TK_PLUS;
                        token->flags |= TF_EQUALS;
                        token->length = 2;
                } else {
                        token->kind = TK_PLUS;
                }

                pos += token->length;
                return;
        }

        if (*pos == '-') {
                if (pos[1] == '>') {
                        token->kind = TK_ARROW;
                        token->length = 2;
                } else if (pos[1] == '-') {
                        token->kind = TK_DECREMENT;
                        token->length = 2;
                } else if (pos[1] == '=') {
                        token->kind = TK_MINUS;
                        token->flags |= TF_EQUALS;
                        token->length = 2;
                } else {
                        token->kind = TK_MINUS;
                }

                pos += token->length;
                return;
        }

        if (*pos == '~') {
                token->kind = TK_TILDE;

                pos += token->length;
                return;
        }

        /* TODO: Use char_info to convert chars to types directly */
        /* These cases can be converted to assignments if followed by a '=' */
        switch(*pos) {
        case '*':
                token->kind = TK_STAR;
                break;
        case '/':
                token->kind = TK_SLASH;
                break;
        case '%':
                token->kind = TK_PERCENT;
                break;
        case '=':
                token->kind = TK_EQUALS;
                break;
        case '!':
                token->kind = TK_NOT;
                break;
        case '<':
                token->kind = TK_LESS_THAN;
                break;
        case '>':
                token->kind = TK_GREATER_THAN;
                break;
        case '^':
                token->kind = TK_CARET;
                break;
        case '&':
                token->kind = TK_AMPERSAND;
                break;
        case '|':
                token->kind = TK_PIPE;
                break;
        default:
                error(token, "bug: unrecognized operator\n");

                pos += token->length;
                return;
        }

        if (pos[1] == '=') {
                token->flags |= TF_EQUALS;
                token->length =  2;
        }

        pos += token->length;
}

static void lex_number_base16(token_t* token)
{
        /* Calculate value of hex number */
        token->value = 0;
        pos += 2;
        while (char_info[*pos] & CHAR_HEX) {
                token->value <<= 4;

                if (char_info[*pos] == CHAR_XUPPER) {
                        token->value |= *pos++ - 'A' + 10;
                } else if (char_info[*pos] == CHAR_XLOWER) {
                        token->value |= *pos++ - 'a' + 10;
                } else {
                        token->value |= *pos++ - '0';
                }
        }
}

static void lex_number_base10(token_t* token)
{
        /* Calculate value of decimal number */
        token->value = *pos - '0';
        pos++;
        while (char_info[*pos] & CHAR_DIGIT) {
                token->value *= 10;
                token->value += *pos++ - '0';
        }
}

static void lex_string(token_t* token)
{
        /* TODO: Escape sequences */

        /* Find end of string */
        pos++;
        while (*pos != '"') {
                pos++;
        }
        pos++;

        token->kind = TK_STRING;
        token->length = (size_t)(pos - token->pos) - 1;
}

static void lex_character(token_t* token)
{
        /* TODO: Escape sequences */

        /* Find end of character */
        pos++;
        while (*pos != '\'') {
                pos++;
        }

        token->kind = TK_CHARACTER;
        token->length = (size_t)(pos - token->pos) - 1;
}

void lexer_next(token_t* token)
{
        skip_whitespace();

        token->flags = TF_NONE;
        token->pos = pos;
        token->line = line;
        token->column = (int)(token->pos - line_start) + 1;

        if (char_info[*pos] & CHAR_ALPHA || *pos == '_') {
                lex_identifier(token);
                return;
        }

        if (char_info[*pos] & CHAR_SINGLE) {
                token->kind = char_info[*pos] >> CHAR_SINGLE_SHIFT;
                token->length = 1;
                pos++;
                return;
        }

        if (char_info[*pos] & CHAR_OPER) {
                lex_operator(token);
                return;
        }

        if (char_info[*pos] & CHAR_DIGIT) {
                token->kind = TK_NUMBER;

                if (pos[0] == '0' && pos[1] == 'x') {
                        lex_number_base16(token);
                } else {
                        lex_number_base10(token);
                }

                token->length = (size_t)(pos - token->pos);
                return;
        }

        if (*pos == '"') {
                lex_string(token);
                return;
        }

        if (*pos == '\'') {
                lex_character(token);
                return;
        }

        if (*pos == '\0') {
                token->kind = TK_EOF;
                return;
        }

        token->kind = TK_UNKNOWN;
        error(token, "unexpected '%c'\n", *token->pos);
}

void lexer_init(char* source)
{
        input = source;
        pos = input;
        line_start = pos;
        line = 1;

        init_keywords();
}
