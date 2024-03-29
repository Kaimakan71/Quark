/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <codegen/codegen.h>

static FILE* out;
static char* arg_regs[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

static void generate_call(ast_node_t* call)
{
        ast_node_t* argument;

        argument = call->children.head;
        for (int n = 0; argument != NULL; n++) {
                if (n < (sizeof(arg_regs) / sizeof(arg_regs[0]))) {
                        fprintf(out, "  mov %s, ", arg_regs[n]);
                } else {
                        fprintf(out, "  pushq ");
                }

                if (argument->kind == NK_NUMBER) {
                        fprintf(out, "%lu\n", argument->value);
                } else if (argument->kind == NK_STRING_REFERENCE) {
                        fprintf(out, "__string%u\n", argument->string->string_id);
                }

                if (argument->next != NULL) {
                        argument = argument->next;
                        continue;
                }

                break;
        }

        fprintf(out, "  call %.*s\n", call->callee->name.length, call->callee->name.string);
}

static void generate_procedure(ast_node_t* procedure)
{
        ast_node_t* node;

        fprintf(out, "\nglobal %.*s\n%.*s:\n  push rbp\n  mov rbp, rsp\n", procedure->name.length, procedure->name.string, procedure->name.length, procedure->name.string);

        node = procedure->children.head;
        while (node != NULL) {
                if (node->kind == NK_CALL) {
                        generate_call(node);
                }

                if (node->next != NULL) {
                        node = node->next;
                        continue;
                }

                break;
        }

        fprintf(out, "  leave\n  ret\n");
}

void codegen(ast_node_t* root, ast_node_t* strings, FILE* _out)
{
        ast_node_t* node;

        out = _out;

        /* Declare strings */
        fprintf(out, "\nsection .data\n");
        node = strings->children.head;
        while (node != NULL) {
                fprintf(out, "__string%u: db \"%.*s\", 0\n", node->string_id, node->string_length, node->string_data);

                node = node->next;
        }

        fprintf(out, "\nsection .text\n");
        node = root->children.head;
        while (node->parent != NULL) {
                if (node->kind == NK_PROCEDURE) {
                        if (node->flags & NF_DEFINITION) {
                                generate_procedure(node);
                        } else {
                                fprintf(out, "extern %.*s\n", node->name.length, node->name.string);
                        }
                }

                if (node->next != NULL) {
                        node = node->next;
                        continue;
                }

                if (node->parent->next != NULL) {
                        node = node->parent->next;
                        continue;
                }

                break;
        }
}
