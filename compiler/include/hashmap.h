/*
 * Hashmap implementation.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _HASHMAP_H
#define _HASHMAP_H

#include "list.h"
#include "hash.h"

typedef struct {
        list_entry_t list_entry;
        hash_t hash;
} hashmap_entry_t;

void hashmap_remove(hashmap_entry_t *entry);
void hashmap_add(list_entry_t *rows, hashmap_entry_t *entry, size_t n_rows);
hashmap_entry_t *hashmap_find(list_entry_t *rows, hash_t hash, size_t n_rows);
void hashmap_init(list_entry_t *rows, size_t n_rows);

#endif /* !_HASHMAP_H */
