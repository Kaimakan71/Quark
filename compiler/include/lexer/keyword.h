/*
 * Lexer keyword storage.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _KEYWORD_H
#define _KEYWORD_H

#include <name.h>
#include <lexer/token.h>

typedef struct keyword {
        name_t name;
        token_kind_t value;

        struct keyword* next;
} keyword_t;

typedef struct {
        keyword_t* head;
        keyword_t* tail;
} keyword_list_t;

keyword_t* find_keyword(token_t* token);
void init_keywords();

#endif /* _KEYWORD_H */
