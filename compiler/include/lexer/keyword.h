/*
 * Lexer keyword storage.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _LEXER_KEYWORD_H
#define _LEXER_KEYWORD_H

#include "hashmap.h"
#include "lexer/token.h"
#include "name.h"

typedef struct {
        hashmap_entry_t hashmap_entry;
        name_t name;
        token_kind_t value;
} keyword_t;

keyword_t* keywords_find(token_t* token);
void keywords_init();

#endif /* !_LEXER_KEYWORD_H */
