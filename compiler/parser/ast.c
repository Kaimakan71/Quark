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

void push_node(ast_node_t* node)
{
        /* Add the node to the doubly linked list */
        if (node->parent->children.head == NULL) {
                node->prev = NULL;
                node->parent->children.head = node;
        } else {
                node->prev = node->parent->children.tail;
                node->parent->children.tail->next = node;
        }

        node->parent->children.tail = node;
}

void delete_node(ast_node_t* top_node)
{
        ast_node_t* current;
        ast_node_t* next;

        current = top_node;
        while (current != NULL) {
                if (current->children.head != NULL) {
                        next = current->children.head;
                        free(current);
                        current = next;
                        continue;
                }

                if (current->next != NULL) {
                        next = current->next;
                        free(current);
                        current = next;
                        continue;
                }

                if (current->parent != top_node && current->parent->next != NULL) {
                        next = current->parent->next;
                        free(current);
                        current = next;
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

ast_node_t* find_node_of_kind(token_t* name, ast_node_t* parent, node_kind_t kind)
{
	ast_node_t* node;

	node = find_node(name, parent);
	if (node == NULL) {
                return NULL;
	}

	if (node->kind != kind) {
		return NULL;
	}

        return node;
}
