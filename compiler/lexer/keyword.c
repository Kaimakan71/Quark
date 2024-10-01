/*
 * Lexer keyword storage.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "hash.h"
#include "lexer/keyword.h"
#include "log.h"

#define KEYWORD_MAP_ROWS 16

static bool initialized = false;
static list_entry_t keyword_map[KEYWORD_MAP_ROWS];

static void create_keyword(char* string, token_kind_t value)
{
        keyword_t* keyword;

        /* Initialize keyword structure */
        keyword = malloc(sizeof(keyword_t));
        keyword->name.string = string;
        keyword->name.length = strlen(string);
        keyword->name.hash = hash_data(keyword->name.string, keyword->name.length);
        keyword->value = value;

        keyword->hashmap_entry.hash = keyword->name.hash;
        hashmap_add(keyword_map, &keyword->hashmap_entry, KEYWORD_MAP_ROWS);
}

keyword_t* keywords_find(token_t* token)
{
        keyword_t* keyword;

        keyword = (keyword_t*)hashmap_find(keyword_map, token->hash, KEYWORD_MAP_ROWS);

        /* Extra check that the lengths match just in case */
        if (keyword != NULL && keyword->name.length != token->length) {
                return NULL;
        }

        return keyword;
}

void keywords_init()
{
        if (initialized) {
                return;
        }

        debug("Initializing keywords...");

        hashmap_init(keyword_map, KEYWORD_MAP_ROWS);
        create_keyword("pub", TK_PUB);
        create_keyword("type", TK_TYPE);
        create_keyword("struct", TK_STRUCT);
        create_keyword("proc", TK_PROC);
        create_keyword("return", TK_RETURN);
        create_keyword("if", TK_IF);

        initialized = true;
}
