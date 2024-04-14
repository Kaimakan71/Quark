/*
 * Keeps track of types.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <hash.h>
#include <parser/type.h>

static ast_node_t* types;

static void create_builtin_type(char* name, size_t bytes, uint8_t flags)
{
        ast_node_t* type;

        type = create_node(types);
        type->kind = NK_BUILTIN_TYPE;
        type->flags |= flags | NF_NAMED;
        type->name.string = name;
        type->name.length = strlen(name);
        type->name.hash = hash_data(name, type->name.length);
        type->bytes = bytes;

        push_node(type, &types->children);
}

ast_node_t* find_type(token_t* name)
{
        return find_node(name, types);
}

void init_types()
{
        types = create_node(NULL);
        types->kind = NK_UNKNOWN;

        create_builtin_type("uint", 8, NF_NONE);
        create_builtin_type("char", 1, NF_NONE);
}
