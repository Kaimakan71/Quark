/*
 * Hashmap implementation.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <stddef.h>
#include "hashmap.h"
#include "log.h"

void hashmap_remove(hashmap_entry_t *entry)
{
        list_remove(&entry->list_entry);
}

void hashmap_add(list_entry_t *rows, hashmap_entry_t *entry, size_t n_rows)
{
        list_prepend(&rows[entry->hash % n_rows], &entry->list_entry);
}

hashmap_entry_t *hashmap_find(list_entry_t *rows, hash_t hash, size_t n_rows)
{
        hashmap_entry_t *head, *entry;

        entry = head = (hashmap_entry_t *)rows[hash % n_rows].next;
        do {
                if (entry->hash == hash) {
                        return entry;
                }

                entry = (hashmap_entry_t *)entry->list_entry.next;
        } while (entry != head);

        return NULL;
}

void hashmap_init(list_entry_t *rows, size_t n_rows)
{
        for (size_t r = 0; r < n_rows; r++) {
                list_init(&rows[r]);
        }
}
