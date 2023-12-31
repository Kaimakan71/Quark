/*
 * Parses arguments and manages compile process.
 * Copyright (c) 2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdio.h>
#include "quark/parser.h"

static char* filename;
static char* input;

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

void dump_tree(ast_node_t* root_node)
{
        ast_node_t* node;
        int indent;

        node = root_node->first_child;
        indent = 0;
        while (1) {
                printf("%*s", indent, "");

                if (node->type == NT_NUMBER) {
                        printf("Number (value=%lu)\n", node->value);

                        if (node->next != NULL) {
                                node = node->next;
                        } else if (node->parent->next != NULL) {
                                node = node->parent->next;
                                indent -= 4;
                        } else {
                                break;
                        }

                        continue;
                }

                switch(node->type) {
                case NT_ADD:
                        printf("Add\n");
                        break;
                case NT_SUBTRACT:
                        printf("Subtract\n");
                        break;
                case NT_MULTIPLY:
                        printf("Multiply\n");
                        break;
                case NT_DIVIDE:
                        printf("Divide\n");
                        break;
                default:
                        printf("Unknown\n");
                        break;
                }
                node = node->first_child;
                indent += 4;
        }
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "No input files\n");
		return -1;
	}

	filename = argv[1];
	input = load_text_file(filename);
	if (input == NULL) {
		perror(filename);
		return -1;
	}

        dump_tree(parse(input));

	free(input);
	return 0;
}
