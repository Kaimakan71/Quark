/*
 * Parses arguments and runs compilation.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codegen/codegen.h>
#include <parser/parser.h>

typedef struct {
        char* name;
        char* description;
        char** value;
} param_t;

char* input_filename = NULL;
char* output_filename = NULL;

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

void parse_args(int argc, char* argv[])
{
        bool found;

        for (int i = 1; i < argc; i++) {
                found = false;

                for (int j = 0; j < (sizeof(params) / sizeof(params[0])); j++) {
                        if (strcmp(argv[i], params[j].name) != 0) {
                                continue;
                        }

                        found = true;

                        if (argc < i + 1) {
                                fprintf(stderr, "expected %s after %s\n", params[j].description, params[j].name);
                                return;
                        }

                        if (*params[j].value != NULL) {
                                fprintf(stderr, "%s cannot be used more than once\n", params[j].name);
                                return;
                        }

                        *params[j].value = argv[++i];
                }

                if (!found) {
                        fprintf(stderr, "Invalid argument \"%s\"\n", argv[i]);
                        return;
                }
        }
}

int main(int argc, char* argv[])
{
        char* input;
        FILE* output_file;
        ast_node_t* root;
        ast_node_t* strings;

        parse_args(argc, argv);

        if (input_filename == NULL || output_filename == NULL) {
                fprintf(stderr, "Both an input and output filename must be set\n");
                return 1;
        }

        output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
                perror(output_filename);
        }

        input = load_text_file(input_filename);
        if (input == NULL) {
                perror(input_filename);
                return -1;
        }

        printf("Parsing...\n");
        root = parse(input, &strings);

        printf("Generating assembly...\n");
        codegen(root, strings, output_file);

        free(input);
        return 0;
}
