/*
 * Runs the operations in an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdio.h>
#include "quark/parser.h"
#include "quark/interperter.h"

static uint64_t get_value(ast_node_t* node)
{
        if (node->type == NT_NUMBER) {
                return node->value;
        }

        if (node->type == NT_VARIABLE) {
            return node->reference->value;
        }

        if (node->type == NT_ADD) {
                return get_value(node->first_child) + get_value(node->last_child);
        }

        if (node->type == NT_SUBTRACT) {
                return get_value(node->first_child) - get_value(node->last_child);
        }

        if (node->type == NT_MULTIPLY) {
                return get_value(node->first_child) * get_value(node->last_child);
        }

        if (node->type == NT_DIVIDE) {
                return get_value(node->first_child) / get_value(node->last_child);
        }
}

void interpert(ast_node_t* root_node)
{
        ast_node_t* node;

        node = root_node->first_child;
        while (node != NULL) {
                if (node->type == NT_VARIABLE) {
                        node->value = get_value(node->first_child);
                        printf("%.*s = %ld\n", node->name.length, node->name.string, node->value);
                }

                node = node->next;
        }
}
