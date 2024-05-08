/*
 * Debug logging.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define DEBUG(s) puts("\033[90m[DEBUG] " __FILE__ ":" STR(__LINE__) ": " s "\033[0m")
#else
#define DEBUG(s)
#endif

#endif /* !_DEBUG_H */
