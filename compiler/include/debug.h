/*
 * Debug output functions.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG_ON

#ifdef DEBUG_ON
#include <stdio.h>
#define DEBUG printf
#else
#define DEBUG(fmt, ...)
#endif /* DEBUG_ON */

#endif /* _DEBUG_H */
