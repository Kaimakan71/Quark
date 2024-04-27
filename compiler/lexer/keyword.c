/*
 * Lexer keyword storage.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stddef.h>
#include <string.h>
#include <debug.h>
#include <hash.h>
#include <lexer/keyword.h>

#define KEYWORD_MAP_ROWS 16

static keyword_list_t keyword_map[KEYWORD_MAP_ROWS];

static void create_keyword(char* string, token_kind_t value)
{
        keyword_t* keyword;
        keyword_list_t* row;

        /* Initialize keyword structure */
        keyword = malloc(sizeof(keyword_t));
        keyword->name.string = string;
        keyword->name.length = strlen(string);
        keyword->name.hash = hash_data(keyword->name.string, keyword->name.length);
        keyword->value = value;
        keyword->next = NULL;

        /* Apend keyword to hashmap row */
        row = &keyword_map[keyword->name.hash % KEYWORD_MAP_ROWS];
        if (row->head == NULL) {
                row->head = keyword;
        } else {
                row->tail->next = keyword;
        }
        row->tail = keyword;
}

keyword_t* find_keyword(token_t* token)
{
        keyword_list_t* row;

        /* Search hashmap row for keyword */
        row = &keyword_map[token->hash % KEYWORD_MAP_ROWS];
        for (keyword_t* keyword = row->head; keyword != NULL; keyword = keyword->next) {
                if (keyword->name.hash == token->hash && keyword->name.length == token->length) {
                        return keyword;
                }
        }

        /* Keyword not found */
        return NULL;
}

void init_keywords()
{
        DEBUG("lexer: Initializing keywords...");

        /* Clear all hashmap rows */
        for (int r = 0; r < KEYWORD_MAP_ROWS; r++) {
                keyword_map[r].head = NULL;
                keyword_map[r].tail = NULL;
        }

        /* Add keywords to map */
        create_keyword("proc", TK_PROC);
        create_keyword("public", TK_PUBLIC);
        create_keyword("return", TK_RETURN);
}
