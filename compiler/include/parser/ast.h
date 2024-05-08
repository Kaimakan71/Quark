/*
 * AST node defintions & management.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_AST_H
#define _PARSER_AST_H

#include <stdint.h>
#include <name.h>
#include <lexer/token.h>

typedef enum {
        NK_UNKNOWN,

        NK_BUILTIN_TYPE,
        NK_TYPE_ALIAS,
        NK_STRUCT,
        NK_STRUCT_MEMBER,

        NK_PROCEDURE,
        NK_PARAMETER,
        NK_RETURN,
        NK_IF,
        NK_CONDITIONS,

        NK_LOCAL_VARIABLE,
        NK_VARIABLE_REFERENCE,
        NK_NUMBER
} node_kind_t;

#define NF_NONE   0
#define NF_NAMED  (1 << 0)
#define NF_PUBLIC (1 << 1)

struct ast_node;

typedef struct {
        struct ast_node* head;
        struct ast_node* tail;
} ast_node_list_t;

typedef struct ast_node {
        node_kind_t kind;
        uint8_t flags;
        name_t name;

        /* Builtin type, struct, storage */
        size_t bytes;

        /* Procedure */
        int n_parameters;
        ast_node_list_t parameters;
        size_t local_size;

        /* Procedure, parameter, local variable */
        struct ast_node* type;
        int pointer_depth;

        /* String */
        int id;
        size_t length;
        char* data;

        /* Enumeration */
        size_t n_values;
        uint64_t* values;

        /* Fields only used by one kind of node */
        union {
                size_t local_offset;          /* Local variable */
                struct ast_node* destination; /* Assignment */
                struct ast_node* callee;      /* Call */
                uint64_t value;               /* Number */
                struct ast_node* variable;    /* Variable reference */
                struct ast_node* string;      /* String reference */
        };

        struct ast_node* parent;
        ast_node_list_t children;
        struct ast_node* prev;
        struct ast_node* next;
} ast_node_t;

ast_node_t* create_node(ast_node_t* parent);
void push_node(ast_node_t* node, ast_node_list_t* list);
void delete_nodes(ast_node_t* top_node);
ast_node_t* find_node(token_t* name, ast_node_t* parent);

#endif /* !_PARSER_AST_H */
