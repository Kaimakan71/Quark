/*
 * Analyzes tokens to create an AST tree.
 * Copyright (c) 2023-2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_PARSER_H
#define _QUARK_PARSER_H

#include <stdint.h>

typedef enum {
        NT_UNKNOWN,

        NT_VARIABLE,
        NT_NUMBER,

        NT_ADD,
        NT_SUBTRACT,
        NT_MULTIPLY,
        NT_DIVIDE
} node_type_t;

typedef struct ast_node {
        node_type_t type;

        union {
                struct {
                        char* string;
                        size_t length;
                        uint32_t hash;
                } name;

                uint64_t value;
        };

        struct ast_node* parent;
        struct ast_node* first_child;
        struct ast_node* last_child;
        struct ast_node* prev;
        struct ast_node* next;
} ast_node_t;

ast_node_t* parse(char* source);

#endif /* _QUARK_PARSER_H */
