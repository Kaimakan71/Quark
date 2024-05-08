/*
 * AST node defintions & management.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stddef.h>
#include <stdlib.h>
#include <error.h>
#include <name.h>
#include <parser/ast.h>

ast_node_t* create_node(ast_node_t* parent)
{
        ast_node_t* node;

        node = malloc(sizeof(ast_node_t));
        node->kind = NK_UNKNOWN;
        node->flags = NF_NONE;
        node->parent = parent;
        node->children.head = NULL;
        node->children.tail = NULL;
        node->next = NULL;

        return node;
}

void push_node(ast_node_t* node, ast_node_list_t* list)
{
        if (list == NULL) {
                list = &node->parent->children;
        }

        /* Add the node to the doubly linked list */
        if (list->head == NULL) {
                node->prev = NULL;
                list->head = node;
        } else {
                node->prev = list->tail;
                list->tail->next = node;
        }

        list->tail = node;
}

void delete_nodes(ast_node_t* top_node)
{
        ast_node_t* node;
        ast_node_t* next;

        node = top_node;
        while (node != NULL) {
                /* Descend if not already visited */
                if (!(node->flags & NF_DELETED) && node->children.head != NULL) {
                        node->flags |= NF_DELETED;
                        node = node->children.head;
                        continue;
                }

                /* Free top node and exit */
                if (node == top_node) {
                        free(node);
                        return;
                }

                /* Go to nearest sibling or parent */
                if (node->next != NULL) {
                        next = node->next
                } else if (node->parent != NULL) {
                        next = node->parent;
                }
                free(node);
                node = next;
        }
}

ast_node_t* find_node(token_t* name, ast_node_t* parent)
{
        /* Search all scopes for node */
        for (ast_node_t* scope = parent; scope != NULL; scope = scope->parent) {
                for (ast_node_t* node = scope->children.head; node != NULL; node = node->next) {
                        if (node->flags & NF_NAMED && node->name.hash == name->hash && node->name.length == name->length) {
                                return node;
                        }
                }
        }

        /* Node not found */
        return NULL;
}
