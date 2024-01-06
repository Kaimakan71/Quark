/*
 * Generates x86_64 assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdio.h>
#include "quark/parser.h"
#include "quark/codegen.h"

void codegen(ast_node_t* root_node, FILE* out)
{
        ast_node_t* node;

        fprintf(out, ".section .data\n");
        node = root_node->first_child;
        while (node != NULL) {
                if (node->type == NT_VARIABLE) {
                        fprintf(out, "%.*s: ", node->name.length, node->name.string);

                        if (node->first_child->type == NT_NUMBER) {
                                fprintf(out, "dq %d\n", node->first_child->value);
                        } else {
                                fprintf(out, "dq 0\n");
                        }
                }

                node = node->next;
        }
}
