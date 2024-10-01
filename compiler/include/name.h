/*
 * Hashed name structure.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _NAME_H
#define _NAME_H

#include <stddef.h>
#include "hash.h"

typedef struct {
        char* string; /* May not be zero-terminated */
        size_t length;
        hash_t hash;
} name_t;

#endif /* !_NAME_H */
