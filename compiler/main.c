/*
 * Parses arguments and runs compilation.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <debug.h>
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

	DEBUG("Loading %s...\n", filename);

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

	DEBUG("Parsing arguments...\n");

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

	return true;
}

int main(int argc, char* argv[])
{
        char* input;
        FILE* output_file;
        ast_node_t* root;
        ast_node_t* strings;

        if (!parse_args(argc, argv)) {
		return -1;
	}

        if (input_filename == NULL || output_filename == NULL) {
                fprintf(stderr, "Both an input and output filename must be set\n");
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
                return -1;
        }

        root = parse(input, &strings);
        codegen(root, strings, output_file);

        free(input);
        return 0;
}
