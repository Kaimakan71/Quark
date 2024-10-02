/*
 * Parses arguments and runs compilation.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "log.h"

typedef struct {
        char* name;
        char* description;
        char** value;
} param_t;

static char* input_filename = NULL;
static char* output_filename = NULL;

static const char* node_kind_strings[] = {
        [NK_UNKNOWN] = "unknown",
        [NK_BUILTIN_TYPE] = "built-in type",
        [NK_TYPE_ALIAS] = "alias type",
        [NK_STRUCT] = "structure type",
        [NK_STRUCT_MEMBER] = "member",
        [NK_PROCEDURE] = "procedure",
        [NK_PARAMETER] = "parameter",
        [NK_CALL] = "call",
        [NK_RETURN] = "return",
        [NK_IF] = "if",
        [NK_CONDITIONS] = "conditions",
        [NK_LOCAL_VARIABLE] = "local variable",
        [NK_VARIABLE_REFERENCE] = "variable reference",
        [NK_NUMBER] = "number"
};

static param_t params[] = {
        { "-i", "input filename", &input_filename },
        { "-o", "output filename", &output_filename }
};

static char* load_text_file(char* filename)
{
        FILE* fp;
        size_t size, n_read;
        char* buf;

        fp = fopen(filename, "rb");
        if (fp == NULL) {
                return NULL;
        }

        /* Get file size */
        fseek(fp, 0, SEEK_END);
        size = (size_t)ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if (size < sizeof(char)) {
                fclose(fp);
                return NULL;
        }

        /* Read entire file */
        buf = malloc(size + sizeof(char));
        n_read = fread(buf, 1, size, fp);
        fclose(fp);
        if (n_read != size) {
                free(buf);
                return NULL;
        }

        /* Terimate string */
        buf[size] = '\0';
        return buf;
}

static bool parse_args(int argc, char* argv[])
{
        for (int i = 1; i < argc; i++) {
                bool found;

                found = false;

                for (int j = 0; j < (int)(sizeof(params) / sizeof(params[0])); j++) {
                        if (strcmp(argv[i], params[j].name) != 0) {
                                continue;
                        }

                        if (argc < i + 1) {
                                fprintf(stderr, "Expected %s after %s\n", params[j].description, params[j].name);
                                return false;
                        }

                        if (*params[j].value != NULL) {
                                fprintf(stderr, "%s was already set\n", params[j].name);
                                return false;
                        }

                        found = true;
                        *params[j].value = argv[++i];
                        break;
                }

                if (!found) {
                        fprintf(stderr, "Invalid argument \"%s\"\n", argv[i]);
                        return false;
                }
        }

        if (input_filename == NULL || output_filename == NULL) {
                fprintf(stderr, "An input filename (-i) and output filename (-o) are required\n");
                return false;
        }

        return true;
}

static void print_node(ast_node_t* node)
{
        if (node->flags & NF_PUBLIC) {
                printf("public ");
        }

        // if (node->flags & NF_NAMED) {
        //         printf("%.*s ", (int)node->name.length, node->name.string);
        // }

        switch (node->kind) {
        case NK_BUILTIN_TYPE:
        case NK_TYPE_ALIAS:
                printf("type %.*s: %lu byte(s), pointer depth %lu\n", (int)node->name.length, node->name.string, node->bytes, node->ptr_depth);
                break;
        case NK_STRUCT:
                printf("type %.*s: struct {\n", (int)node->name.length, node->name.string);
                break;
        case NK_STRUCT_MEMBER:
        case NK_LOCAL_VARIABLE:
                printf("%.*s %.*s;\n", (int)node->type->name.length, node->type->name.string, (int)node->name.length, node->name.string);
                break;
        case NK_PROCEDURE:
                printf("proc %.*s()", (int)node->name.length, node->name.string);
                if (node->type != NULL) {
                        printf(" -> %.*s", (int)node->type->name.length, node->type->name.string);
                }
                printf(" {\n");
                break;
        case NK_PARAMETER:
                printf("%.*s %.*s; (parameter)\n", (int)node->type->name.length, node->type->name.string, (int)node->name.length, node->name.string);
                break;
        case NK_CALL:
                printf("%.*s();\n", (int)node->variable->name.length, node->variable->name.string);
                break;
        case NK_NUMBER:
                printf("0x%lx\n", node->value);
                break;
        case NK_VARIABLE_REFERENCE:
                printf("%.*s\n", (int)node->variable->name.length, node->variable->name.string);
                break;
        default:
                printf("%s\n", node_kind_strings[node->kind]);
                break;
        }
}

static void print_tree(ast_node_t* root)
{
        ast_node_t* node;
        int indent;

        root->flags |= NF_VISITED;
        node = root->children.head;
        indent = 0;
        while (node != NULL) {
                if (!(node->flags & NF_VISITED)) {
                        printf("%*s", indent, "");
                        print_node(node);

                        node->flags |= NF_VISITED;
                        if (node->children.head != NULL) {
                                node = node->children.head;
                                indent += 4;
                                continue;  
                        }
                }

                if (node->next != NULL) {
                        node = node->next;
                        continue;
                }

                if (node->parent != NULL) {
                        node = node->parent;
                        indent -= 4;

                        if (node->kind == NK_PROCEDURE || node->kind == NK_STRUCT) {
                                printf("%*s}\n", indent, "");
                        }

                        continue;
                }

                break;
        }
}

int main(int argc, char* argv[])
{
        parser_t parser;
        char* input;

        if (!parse_args(argc, argv)) {
                return -1;
        }

        input = load_text_file(input_filename);
        if (input == NULL) {
                perror(input_filename);
                return -1;
        }

        /* Parse and print tree */
        parser_init(&parser, input);
        parser_parse(&parser);
        print_tree(parser.types);
        print_tree(parser.procedures);
        parser_destory(&parser);

        free(input);
        return 0;
}
