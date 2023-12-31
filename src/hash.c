/*
 * 32-bit FNV-1a hash function.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdint.h>
#include "quark/hash.h"

uint32_t hash_data(void* data, size_t length)
{
	uint32_t hash;

	hash = FNV_OFFSET_BASIS;
	for (size_t i = 0; i < length; i++) {
		hash ^= *(uint8_t*)(data + i);
		hash *= FNV_PRIME;
	}

	return hash;
}
