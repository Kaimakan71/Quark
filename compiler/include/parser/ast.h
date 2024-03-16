/*
 * AST node defintions & management.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _AST_H
#define _AST_H

#include <stdint.h>
#include <name.h>
#include <lexer/token.h>

typedef enum {
        NK_BUILTIN_TYPE,
        NK_PROCEDURE,
        NK_PARAMETER,
        NK_CALL,
	NK_NUMBER,
        NK_STRING,
        NK_STRING_REFERENCE
} node_kind_t;

#define NF_NONE 0
#define NF_NAMED (1 << 0)
#define NF_CHARACTER (1 << 1)
#define NF_DEFINITION (1 << 2)

typedef struct ast_node {
        node_kind_t kind;
        uint8_t flags;
        name_t name;

        /* Builtin type */
        size_t bits;

        /* Parameter */
        struct ast_node* type;
        size_t pointer_depth;

        /* Procedure call */
        struct ast_node* callee;

	/* Number */
	uint64_t value;

        /* String */
        unsigned int string_id;
        size_t string_length;
        char* string_data;

        /* String reference */
        struct ast_node* string;

        struct ast_node* parent;
        struct {
                struct ast_node* head;
                struct ast_node* tail;
        } children;
        struct ast_node* prev;
        struct ast_node* next;
} ast_node_t;

ast_node_t* create_node(ast_node_t* parent);
void push_node(ast_node_t* node);
void delete_node(ast_node_t* top_node);
ast_node_t* find_node(token_t* name, ast_node_t* parent);
ast_node_t* find_node_of_kind(token_t* name, ast_node_t* parent, node_kind_t kind);

#endif /* _AST_H */
