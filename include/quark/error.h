/*
 * Error output functions.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_ERROR_H
#define _QUARK_ERROR_H

#include "quark/lexer.h"

void error(token_t* token, char* fmt, ...);

#endif /* _QUARK_ERROR_H */
