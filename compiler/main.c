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
        [NK_UNKNOWN] = "Unknown",
        [NK_BUILTIN_TYPE] = "Type",
        [NK_STRUCT] = "Struct",
        [NK_STRUCT_MEMBER] = "Struct member",
        [NK_PROCEDURE] = "Procedure",
        [NK_PARAMETER] = "Parameter",
        [NK_RETURN] = "Return",
        [NK_IF] = "If",
        [NK_CONDITIONS] = "Conditions",
        [NK_LOCAL_VARIABLE] = "Local variable",
        [NK_VARIABLE_REFERENCE] = "Variable reference",
        [NK_NUMBER] = "Number"
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
                                fprintf(stderr, "Cannot use argument %s more than once\n", params[j].name);
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
                fprintf(stderr, "Both an input and output filename must be set\n");
                return false;
        }

        return true;
}

static void print_tree(ast_node_t* root)
{
        ast_node_t* node;
        int indent;

        node = root->children.head;
        indent = 0;
        while (node != NULL) {
                printf("%*s %s ", indent, "", node_kind_strings[node->kind]);

                if (node->flags & NF_NAMED) {
                        printf("%.*s ", node->name.length, node->name.string);
                }

                switch (node->kind) {
                case NK_BUILTIN_TYPE:
                        printf("(%lu bytes)", node->bytes);
                        break;
                case NK_PROCEDURE:
                        printf("(%s)", node->flags & NF_PUBLIC ? "public":"private");
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
                        printf("to %.*s", node->variable->name.length, node->variable->name.string);
                        break;
                }

                printf("\n");

                if (node->children.head != NULL) {
                        node = node->children.head;
                        indent += 4;
                        continue;
                }

                if (node->next != NULL) {
                        node = node->next;
                        continue;
                }

                if (node->parent != NULL && node->parent->next != NULL) {
                        node = node->parent->next;
                        indent -= 4;
                        continue;
                }

                if (node->parent->parent != NULL && node->parent->parent->next != NULL) {
                        node = node->parent->parent->next;
                        indent -= 8;
                        continue;
                }

                break;
        }
}

int main(int argc, char* argv[])
{
        FILE* output_file;
        char* input;
        parser_t* parser;

        if (!parse_args(argc, argv)) {
                return -1;
        }

        output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
                perror(output_filename);
                return -1;
        }

        input = load_text_file(input_filename);
        if (input == NULL) {
                perror(input_filename);
                fclose(output_file);
                return -1;
        }

        parser = create_parser(input);
        if (parser == NULL) {
                fclose(output_file);
                free(input);
                return -1;
        }

        parser_parse(parser);

        print_tree(parser->types);
        print_tree(parser->procedures);

        if (!codegen(parser, output_file)) {
                fprintf(stderr, "Error encountered while generating output\n");
        }

        parser_destory(parser);

        fclose(output_file);
        free(input);
        return 0;
}
