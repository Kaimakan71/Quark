/*
 * Parses arguments and manages compile process.
 * Copyright (c) 2023-2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdio.h>
#include "quark/parser.h"
#include "quark/interperter.h"

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

        interpert(parse(input));

	free(input);
	return 0;
}
