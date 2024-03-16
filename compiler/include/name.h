/*
 * Hashed name structure.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _NAME_H
#define _NAME_H

#include <stdint.h>

typedef struct {
        char* string; /* May not be zero-terminated */
        size_t length;
        uint32_t hash;
} name_t;

#endif /* _NAME_H */
