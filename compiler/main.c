/*
 * Parses arguments and runs compilation.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codegen/codegen.h>
#include <parser/parser.h>

char* load_text_file(char* filename)
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

int main(int argc, char* argv[])
{
        char* input_filename = NULL;
        char* output_filename = NULL;
        char* input;
        FILE* output_file;
        ast_node_t* root;
        ast_node_t* strings;

        if (argc < 2) {
                fprintf(stderr, "Not enough arguments\n");
                return 1;
        }

        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        if (argc < i + 1) {
                                fprintf(stderr, "-c must be followed by the input filename\n");
                                return 1;
                        }

                        if (input_filename != NULL) {
                                fprintf(stderr, "Input filename already specified\n");
                                return 1;
                        }

                        input_filename = argv[++i];
                        continue;
                }

                if (strcmp(argv[i], "-o") == 0) {
                        if (argc < i + 1) {
                                fprintf(stderr, "-o must be followed by the output filename\n");
                                return 1;
                        }

                        if (output_filename != NULL) {
                                fprintf(stderr, "Output filename already specified\n");
                                return 1;
                        }

                        output_filename = argv[++i];
                        continue;
                }

                fprintf(stderr, "Unrecognized option \"%s\"\n", argv[i]);
                return 1;
        }

        if (input_filename == NULL || output_filename == NULL) {
                fprintf(stderr, "Input and output filenames must be specified\n");
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
