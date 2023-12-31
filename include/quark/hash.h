/*
 * 32-bit FNV-1a hash function.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_HASH_H
#define _QUARK_HASH_H

#include <stdint.h>
#include <stdlib.h>

#define FNV_PRIME        0x01000193
#define FNV_OFFSET_BASIS 0x811c9dc5

uint32_t hash_data(void* data, size_t length);

#endif /* _QUARK_HASH_H */
