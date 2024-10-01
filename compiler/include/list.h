/*
 * Doubly-linked list implementation.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _LIST_H
#define _LIST_H

typedef struct list_entry {
        struct list_entry *prev;
        struct list_entry *next;
} list_entry_t;

static inline void list_append(list_entry_t *head, list_entry_t *entry)
{
        entry->prev = head->prev;
        entry->next = head;
        head->prev->next = entry;
        head->prev = entry;
}

static inline void list_prepend(list_entry_t *head, list_entry_t *entry)
{
        entry->next = head->next;
        entry->prev = head;
        head->next->prev = entry;
        head->next = entry;
}

static inline void list_remove(list_entry_t *entry)
{
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
}

static inline void list_init(list_entry_t *head)
{
        head->prev = head;
        head->next = head;
}

#endif /* !_LIST_H */
