/*
 * Keeps track of types.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <error.h>
#include <hash.h>
#include <parser/type.h>

static void create_builtin_type(ast_node_t* root, char* name, size_t size, uint8_t flags)
{
        ast_node_t* type;

        type = create_node(root);
        type->kind = NK_BUILTIN_TYPE;
        type->flags |= flags | NF_NAMED;
        type->name.string = name;
        type->name.length = strlen(name);
        type->name.hash = hash_data(name, type->name.length);
        type->size = size;

        push_node(type);
}

void init_types(ast_node_t* root)
{

        DEBUG("Initializing types...\n");

        create_builtin_type(root, "uint", 8, NF_NONE);
        create_builtin_type(root, "char", 1, NF_NONE);
}
