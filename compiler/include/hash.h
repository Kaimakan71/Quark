/*
 * FNV-1a hash function.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _HASH_H
#define _HASH_H

#include <stdint.h>
#include <stdlib.h>

#define FNV_PRIME        0x01000193
#define FNV_OFFSET_BASIS 0x811c9dc5

typedef uint32_t hash_t;

hash_t hash_data(void* data, size_t length);
hash_t hash_string(char *str);

#endif /* !_HASH_H */
