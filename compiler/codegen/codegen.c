/*
 * Generates assembly from an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <debug.h>
#include <codegen.h>

static void generate_return(ast_node_t* statement, FILE* out)
{
        /* TODO: Return values */

        fprintf(out, "\tretq\n");
}

static void generate_statements(ast_node_t* statements, FILE* out)
{
        ast_node_t* statement;

        DEBUG("Generating statements...");

        statement = statements->children.head;
        while (statement != NULL) {
                if (statement->kind == NK_RETURN) {
                        generate_return(statement, out);
                }

                statement = statement->next;
        }
}

static void generate_procedure(ast_node_t* procedure, FILE* out)
{
        DEBUG("Generating procedure...");

        fprintf(out, ".globl %.*s\n", procedure->name.length, procedure->name.string);
        fprintf(out, ".type %.*s, @function\n", procedure->name.length, procedure->name.string);
        fprintf(out, "%.*s:\n", procedure->name.length, procedure->name.string);

        if (procedure->children.head != NULL) {
                generate_statements(procedure, out);
        }

        /* Generate return if needed */
        if (procedure->children.tail == NULL || procedure->children.tail->kind != NK_RETURN) {
                generate_return(NULL, out);
        }

        fprintf(out, ".size %.*s, .-%.*s\n\n", procedure->name.length, procedure->name.string, procedure->name.length, procedure->name.string);
}

static void generate_procedures(ast_node_t* procedures, FILE* out)
{
        ast_node_t* procedure;

        DEBUG("Generating procedures...");

        if (procedures->children.head == NULL) {
                return;
        }

        fprintf(out, ".text\n\n");
        procedure = procedures->children.head;
        while (procedure != NULL) {
                if (procedure->children.head != NULL) {
                        generate_procedure(procedure, out);
                } else {
                        fprintf(out, "extern %.*s\n\n", procedure->name.length, procedure->name.string);
                }

                procedure = procedure->next;
        }
}

bool codegen(parser_t* parser, FILE* out)
{
        DEBUG("Generating assembly...");

        generate_procedures(parser->procedures, out);

        return true;
}
