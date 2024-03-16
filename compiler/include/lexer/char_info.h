/*
 * Character info table.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _CHAR_INFO_H
#define _CHAR_INFO_H

#include <stdint.h>
#include <lexer/token.h>

#define CHAR_HORZ_WS (1 << 0)
#define CHAR_VERT_WS (1 << 1)
#define CHAR_DIGIT   (1 << 2)
#define CHAR_XDIGIT  (1 << 3)
#define CHAR_UPPER   (1 << 4)
#define CHAR_LOWER   (1 << 5)
#define CHAR_OPER    (1 << 6)
#define CHAR_SINGLE  (1 << 7)

#define CHAR_HEX        (CHAR_DIGIT | CHAR_XDIGIT)
#define CHAR_XUPPER     (CHAR_XDIGIT | CHAR_UPPER)
#define CHAR_XLOWER     (CHAR_XDIGIT | CHAR_LOWER)
#define CHAR_WHITESPACE (CHAR_HORZ_WS | CHAR_VERT_WS)
#define CHAR_ALPHA      (CHAR_UPPER | CHAR_LOWER)
#define CHAR_ALNUM      (CHAR_ALPHA | CHAR_DIGIT)

#define CHAR_SINGLE_SHIFT 8
#define MAKE_SINGLE(type) ((type << CHAR_SINGLE_SHIFT) | CHAR_SINGLE)
#define CHAR_COMMA   MAKE_SINGLE(TK_COMMA)
#define CHAR_DOT     MAKE_SINGLE(TK_DOT)
#define CHAR_COLON   MAKE_SINGLE(TK_COLON)
#define CHAR_SEMI    MAKE_SINGLE(TK_SEMICOLON)
#define CHAR_LPAREN  MAKE_SINGLE(TK_LPAREN)
#define CHAR_RPAREN  MAKE_SINGLE(TK_RPAREN)
#define CHAR_LCURLY  MAKE_SINGLE(TK_LCURLY)
#define CHAR_RCURLY  MAKE_SINGLE(TK_RCURLY)
#define CHAR_LSQUARE MAKE_SINGLE(TK_LSQUARE)
#define CHAR_RSQUARE MAKE_SINGLE(TK_RSQUARE)

extern uint16_t char_info[256];

#endif /* _CHAR_INFO_H */
