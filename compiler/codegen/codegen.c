/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <string.h>
#include <codegen/codegen.h>

static FILE* out;
static char* arg_regs[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

#define N_ARG_REGS (sizeof(arg_regs) / sizeof(arg_regs[0]))

static void generate_call(ast_node_t* call);

static void generate_value(char* destination, ast_node_t* value)
{
        if (value->kind == NK_CALL) {
                generate_call(value);

                if (strcmp(destination, "rax") != 0) {
                        fprintf(out, "  mov %s, rax\n", destination);
                }
        } else if (value->kind == NK_VARIABLE_REFERENCE) {
                fprintf(out, "  mov %s, [rsp+0x%lx]\n", destination, value->variable->local_offset);
        } else if (value->kind == NK_NUMBER) {
                fprintf(out, "  mov %s, 0x%lx\n", destination, value->value);
        } else if (value->kind == NK_STRING_REFERENCE) {
                fprintf(out, "  lea rax, [rel __string%u]\n", value->string->id);

                if (strcmp(destination, "rax") != 0) {
                        fprintf(out, "  mov %s, rax\n", destination);
                }
        }
}

static void generate_call(ast_node_t* call)
{
        ast_node_t* argument;
        int reg_index;
        char* destination;

        /* Perform calls first to avoid overwriting regs */
        /* TODO: This does not *always* work */
        argument = call->children.head;
        reg_index = 0;
        while (argument != NULL) {
                if (argument->kind != NK_CALL) {
                        argument = argument->next;
                        reg_index++;
                        continue;
                }

                generate_call(argument);
                fprintf(out, "  mov %s, rax\n", arg_regs[reg_index]);

                argument = argument->next;
                reg_index++;
        }

        argument = call->children.head;
        reg_index = 0;
        destination = malloc(sizeof(char) * 16);
        while (argument != NULL) {
                if (argument->kind == NK_CALL) {
                        argument = argument->next;
                        reg_index++;
                        continue;
                }

                snprintf(destination, 16, "%s", arg_regs[reg_index]);
                generate_value(destination, argument);

                argument = argument->next;
                reg_index++;
        }
        free(destination);

        fprintf(out, "  call %.*s\n", call->callee->name.length, call->callee->name.string);
}

static void generate_assignment(ast_node_t* assignment)
{
        char* destination;

        destination = malloc(sizeof(char) * 16);

        snprintf(destination, 16, "qword [rsp+0x%lx]", assignment->destination->local_offset);
        generate_value(destination, assignment->children.head);

        free(destination);
}

static void generate_return(ast_node_t* ret, ast_node_t* procedure)
{
        if (ret->children.head != NULL) {
                generate_value("rax", ret->children.head);
        }

        if (procedure->local_size > 0) {
                fprintf(out, "  leave\n  ret\n");
        } else {
                fprintf(out, "  pop rbp\n  ret\n");
        }
}

static void generate_procedure(ast_node_t* procedure)
{
        ast_node_t* node;

        fprintf(out, "global %.*s\n%.*s:\n  push rbp\n  mov rbp, rsp\n", procedure->name.length, procedure->name.string, procedure->name.length, procedure->name.string);
        if (procedure->local_size > 0) {
                fprintf(out, "  sub rsp, 0x%lx\n", procedure->local_size);
        }

        node = procedure->children.head;
        while (node != NULL) {
                if (node->kind == NK_RETURN) {
                        generate_return(node, procedure);

                        /* If a return statement always runs, nothing will happen after */
                        return;
                } else if (node->kind == NK_CALL) {
                        generate_call(node);
                } else if (node->kind == NK_ASSIGNMENT) {
                        generate_assignment(node);
                }

                node = node->next;
        }

        if (procedure->local_size > 0) {
                fprintf(out, "  leave\n  ret\n");
        } else {
                fprintf(out, "  pop rbp\n  ret\n");
        }
}

static void generate_procedures(ast_node_t* procedures)
{
        ast_node_t* procedure;

        if (procedures->children.head == NULL) {
                return;
        }

        fprintf(out, "section .text\n\n");
        procedure = procedures->children.head;
        while (procedure != NULL) {
                if (procedure->children.head != NULL) {
                        generate_procedure(procedure);
                } else {
                        fprintf(out, "extern %.*s\n\n", procedure->name.length, procedure->name.string);
                }

                procedure = procedure->next;
        }
}

static void generate_strings(ast_node_t* strings)
{
        ast_node_t* string;

        if (strings->children.head == NULL) {
                return;
        }

        fprintf(out, "section .data\n\n");
        string = strings->children.head;
        while (string != NULL) {
                fprintf(out, "__string%u: db \"%.*s\", 0\n", string->id, string->length, string->data);
                string = string->next;
        }

        fprintf(out, "\n");
}

void codegen(ast_node_t* procedures, ast_node_t* strings, FILE* _out)
{
        ast_node_t* node;

        out = _out;

        generate_strings(strings);
        generate_procedures(procedures);
}
