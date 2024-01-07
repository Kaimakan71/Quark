/*
 * Analyzes tokens to create an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _QUARK_PARSER_H
#define _QUARK_PARSER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
        NT_UNKNOWN,

        NT_FUNCTION,
        NT_FUNCTION_CALL,
        NT_VARIABLE,
        NT_NUMBER,

        NT_ADD,
        NT_SUBTRACT,
        NT_MULTIPLY,
        NT_DIVIDE
} node_type_t;

#define TF_NONE 0
#define TF_SIGNED (1 << 0)

typedef struct {
        char* string;
        size_t length;
        uint32_t hash;
} name_t;

typedef struct data_type {
	name_t name;
	size_t bits;
	uint8_t flags;

	struct data_type* next;
} data_type_t;

typedef struct ast_node {
        node_type_t type;
	name_t name;
	data_type_t* data_type;
	uint64_t value;
	struct ast_node* reference;

        struct ast_node* parent;
        struct ast_node* first_child;
        struct ast_node* last_child;
        struct ast_node* prev;
        struct ast_node* next;
} ast_node_t;

ast_node_t* parse(char* source);

#endif /* _QUARK_PARSER_H */
