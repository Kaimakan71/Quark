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
        node->flags = NF_NONE;
        node->parent = parent;
        node->children.head = NULL;
        node->children.tail = NULL;
        node->next = NULL;

        return node;
}

void push_node(ast_node_t* node, ast_node_list_t* list)
{
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
                if (node->children.head != NULL) {
                        next = node->children.head;
                        free(node);
                        node = next;
                        continue;
                }

                if (node->next != NULL) {
                        next = node->next;
                        free(node);
                        node = next;
                        continue;
                }

                if (node->parent != top_node && node->parent->next != NULL) {
                        next = node->parent->next;
                        free(node);
                        node = next;
                        continue;
                }

                break;
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
