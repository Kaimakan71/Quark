/*
 * Parses arguments and runs compilation.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <parser/ast.h>
#include <codegen.h>

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
        [NK_TYPE_ALIAS] = "type alias",
        [NK_STRUCT] = "struct",
        [NK_STRUCT_MEMBER] = "member",
        [NK_PROCEDURE] = "proc",
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
        FILE* file;
        long size;
        char* buf;

        file = fopen(filename, "rb");
        if (file == NULL) {
                return NULL;
        }

        /* Get file size */
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (size < 1) {
                fclose(file);
                return NULL;
        }

        /* Read entire file */
        buf = malloc(size + 1);
        if (fread(buf, 1, size, file) != (size_t)size) {
                free(buf);
                fclose(file);
                return NULL;
        }

        fclose(file);
        buf[size] = '\0';
        return buf;
}

static bool parse_args(int argc, char* argv[])
{
        bool found;

        for (int i = 1; i < argc; i++) {
                found = false;

                for (int j = 0; j < (sizeof(params) / sizeof(params[0])); j++) {
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
                printf("pub ");
        }

        printf("%s ", node_kind_strings[node->kind]);

        if (node->flags & NF_NAMED) {
                printf("%.*s ", node->name.length, node->name.string);
        }

        switch (node->kind) {
        case NK_BUILTIN_TYPE:
        case NK_TYPE_ALIAS:
                printf("(%lu byte(s), ptr depth %lu)", node->bytes, node->pointer_depth);
                break;
        case NK_PARAMETER:
        case NK_STRUCT_MEMBER:
        case NK_LOCAL_VARIABLE:
                printf("(%.*s)", node->type->name.length, node->type->name.string);
                break;
        case NK_NUMBER:
                printf("%lx", node->value);
                break;
        case NK_VARIABLE_REFERENCE:
        case NK_CALL:
                printf("to %.*s", node->variable->name.length, node->variable->name.string);
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
                        printf("\n");

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
                        continue;
                }

                break;
        }
}

int main(int argc, char* argv[])
{
        char* input;
        codegen_t generator;

        if (!parse_args(argc, argv)) {
                return -1;
        }

        input = load_text_file(input_filename);
        if (input == NULL) {
                perror(input_filename);
                return -1;
        }

        generator.out = fopen(output_filename, "w");
        if (generator.out == NULL) {
                perror(output_filename);
                free(input);
                return -1;
        }

        generator.parser = create_parser(input);
        if (generator.parser == NULL) {
                fclose(generator.out);
                free(input);
                return -1;
        }

        parser_parse(generator.parser);

        print_tree(generator.parser->types);
        print_tree(generator.parser->procedures);

        /*
        generator.bytes = 8;
        if (!codegen(&generator)) {
                fprintf(stderr, "Error encountered while generating output\n");
        }
        */

        parser_destory(generator.parser);

        fclose(generator.out);
        free(input);
        return 0;
}
