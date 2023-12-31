/*
 * Character info table.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_CHAR_INFO_H
#define _QUARK_CHAR_INFO_H

#include <stdint.h>

#define CHAR_HORZ_WS (1 << 0)
#define CHAR_VERT_WS (1 << 1)
#define CHAR_DIGIT   (1 << 2)
#define CHAR_XDIGIT  (1 << 3)
#define CHAR_UPPER   (1 << 4)
#define CHAR_LOWER   (1 << 5)
#define CHAR_SPECIAL (1 << 6)

#define CHAR_XUPPER     (CHAR_XDIGIT | CHAR_UPPER)
#define CHAR_XLOWER     (CHAR_XDIGIT | CHAR_LOWER)
#define CHAR_WHITESPACE (CHAR_HORZ_WS | CHAR_VERT_WS)
#define CHAR_ALPHA      (CHAR_UPPER | CHAR_LOWER)
#define CHAR_ALNUM      (CHAR_ALPHA | CHAR_DIGIT)

extern uint16_t char_info[256];

#endif /* _QUARK_CHAR_INFO_H */
