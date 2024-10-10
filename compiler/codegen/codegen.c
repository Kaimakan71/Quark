/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include "codegen.h"
#include "log.h"

static char* arg_reg_bases[] = { "di", "si", "d", "c", "r8", "r9" };

static void generate_arg_reg(int id, size_t bytes, FILE* fp)
{
        /* r[8-9][d|w|b]? */
        if (id >= 4) {
                fputs(arg_reg_bases[id], fp);

                switch (bytes) {
                case 8:
                        break;
                case 4:
                        fputc('d', fp);
                        break;
                case 2:
                        fputc('w', fp);
                        break;
                case 1:
                        fputc('b', fp);
                        break;
                default:
                        break;
                }

                return;
        }

        /* [r|e][di|si|dx|cx] */
        if (bytes == 8) {
                fputc('r', fp);
        } else if (bytes == 4) {
                fputc('e', fp);
        }

        fputs(arg_reg_bases[id], fp);

        if (bytes == 1) {
                /* [di|si|d|c]l */
                fputc('l', fp);
        } if (id >= 2) {
                /* [d|c]x */
                fputc('x', fp);
        }
}

static void generate_call(ast_node_t* call, FILE* fp, size_t word_bytes)
{
        fprintf(
                fp,
                "\tcall %.*s\n",
                (int)call->callee->name.length, call->callee->name.string
        );
}

static void generate_statements(ast_node_t* parent, FILE* fp, size_t word_bytes)
{
        for (ast_node_t* node = parent->children.head; node != NULL; node = node->next) {
                if (node->kind == NK_CALL) {
                        generate_call(node, fp, word_bytes);
                }
        }
}

static void generate_procedure(ast_node_t* procedure, FILE* fp, size_t word_bytes)
{
        fprintf(
                fp,
                "\t.globl %.*s\n"
                "\t.type %.*s, %%function\n"
                "%.*s:\n",
                (int)procedure->name.length, procedure->name.string,
                (int)procedure->name.length, procedure->name.string,
                (int)procedure->name.length, procedure->name.string
        );

        generate_statements(procedure, fp, word_bytes);

        fprintf(
                fp,
                "\t.size %.*s, .-%.*s\n",
                (int)procedure->name.length, procedure->name.string, (int)procedure->name.length, procedure->name.string
        );
}

bool codegen(ast_node_t* procedures, FILE* fp, size_t word_bytes)
{
        debug("Generating assembly code...");

        fprintf(fp, "\t.text\n");
        for (ast_node_t* proc = procedures->children.head; proc != NULL; proc = proc->next) {
                if (!(proc->flags & NF_DEFINITION)) {
                        continue;
                }

                generate_procedure(proc, fp, word_bytes);
        }

        return true;
}
