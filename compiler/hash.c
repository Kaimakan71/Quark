/*
 * FNV-1a hash function.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include "hash.h"

hash_t hash_data(void* data, size_t length)
{
        hash_t hash;

        hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < length; i++) {
                hash ^= ((uint8_t*)data)[i];
                hash *= FNV_PRIME;
        }

        return hash;
}

hash_t hash_string(char *str)
{
        hash_t hash;

        hash = FNV_OFFSET_BASIS;
        while (*str) {
                hash ^= (uint8_t)*str++;
                hash *= FNV_PRIME;
        }

        return hash;
}
