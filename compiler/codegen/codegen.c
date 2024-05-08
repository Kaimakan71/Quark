/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <debug.h>
#include <codegen.h>

typedef enum {
        REG_ACCUMULATOR = 0,
        REG_BASE,
        REG_COUNTER,
        REG_DATA,
        REG_STACK,
        REG_STACK_BASE,
        REG_DESTINATION,
        REG_SOURCE
} reg_id_t;

static char* reg_names[][4] = {
        [REG_ACCUMULATOR] = { "al", "ax", "eax", "rax" },
        [REG_BASE]        = { "bl", "bx", "ebx", "rbx" },
        [REG_COUNTER]     = { "cl", "cx", "ecx", "rcx" },
        [REG_DATA]        = { "dl", "dx", "edx", "rdx" },
        [REG_STACK]       = { "spl", "sp", "esp", "rsp" },
        [REG_STACK_BASE]  = { "bpl", "bp", "ebp", "rbp" },
        [REG_DESTINATION] = { "dil", "di", "edi", "rdi" },
        [REG_SOURCE]      = { "sil", "si", "esi", "rsi" }
};

static inline char* get_reg_name(reg_id_t id, uint8_t bytes)
{
        return reg_names[id][__builtin_ctz(bytes)];
}

static void generate_value(codegen_t* generator, ast_node_t* value, uint8_t bytes)
{
        char* accumulator;

        if (value->kind == NK_NUMBER) {
                accumulator = get_reg_name(REG_ACCUMULATOR, bytes);

                if (value->value == 0) {
                        fprintf(generator->out, "\txor %s, %s\n", accumulator, accumulator);
                } else {
                        fprintf(generator->out, "\tmov %s, %lx\n", accumulator, value->value);
                }

                return;
        }

        if (value->kind == NK_VARIABLE_REFERENCE) {
                char* stack_base;

                accumulator = get_reg_name(REG_ACCUMULATOR, value->variable->type->bytes);
                stack_base = get_reg_name(REG_STACK_BASE, generator->bytes);

                fprintf(generator->out, "\tmov %s, [%s - %lx]\n", accumulator, stack_base, value->variable->local_offset);

                return;
        }
}

static void generate_return(codegen_t* generator, ast_node_t* statement)
{
        if (statement != NULL && statement->children.head != NULL) {
                generate_value(generator, statement->children.head, statement->type->bytes);
        }

        fprintf(generator->out, "\tpop %s\n\tret\n", get_reg_name(REG_STACK_BASE, generator->bytes));
}

static void generate_prologue(codegen_t* generator, ast_node_t* procedure)
{
        char* stack;
        char* stack_base;

        stack = get_reg_name(REG_STACK, generator->bytes);
        stack_base = get_reg_name(REG_STACK_BASE, generator->bytes);
        fprintf(generator->out, "\tpush %s\n\tmov %s, %s\n", stack_base, stack_base, stack);

        if (procedure->local_size > 0) {
                fprintf(generator->out, "\tsub %s, %lx\n", stack, procedure->local_size);
        }
}

static void generate_statements(codegen_t* generator, ast_node_t* statements)
{
        ast_node_t* statement;

        DEBUG("Generating statements...");

        statement = statements->children.head;
        while (statement != NULL) {
                if (statement->kind == NK_RETURN) {
                        generate_return(generator, statement);
                }

                statement = statement->next;
        }
}

static void generate_procedure(codegen_t* generator, ast_node_t* procedure)
{
        DEBUG("Generating procedure...");

        fprintf(generator->out, "\tglobal %.*s\n", procedure->name.length, procedure->name.string);
        fprintf(generator->out, "\talign 16\n");
        fprintf(generator->out, "%.*s:\n", procedure->name.length, procedure->name.string);

        generate_prologue(generator, procedure);

        if (procedure->children.head != NULL) {
                generate_statements(generator, procedure);
        }

        /* Generate return if needed */
        if (procedure->children.tail == NULL || procedure->children.tail->kind != NK_RETURN) {
                generate_return(generator, NULL);
        }
}

static void generate_procedures(codegen_t* generator, ast_node_t* procedures)
{
        ast_node_t* procedure;

        DEBUG("Generating procedures...");

        if (procedures->children.head == NULL) {
                return;
        }

        fprintf(generator->out, "\tsection .text\n\n");
        procedure = procedures->children.head;
        while (procedure != NULL) {
                if (procedure->children.head != NULL) {
                        generate_procedure(generator, procedure);
                } else {
                        fprintf(generator->out, "extern %.*s\n\n", procedure->name.length, procedure->name.string);
                }

                procedure = procedure->next;
        }
}

bool codegen(codegen_t* generator)
{
        DEBUG("Generating assembly...");

        generate_procedures(generator, generator->parser->procedures);

        return true;
}
