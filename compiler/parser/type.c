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
#include <parser/storage.h>

static void create_builtin_type(ast_node_t* types, char* name, size_t bytes, uint8_t flags)
{
        ast_node_t* type;

        type = create_node(types);
        type->kind = NK_BUILTIN_TYPE;
        type->flags |= flags | NF_NAMED;
        type->name.string = name;
        type->name.length = strlen(name);
        type->name.hash = hash_data(name, type->name.length);
        type->bytes = bytes;

        push_node(type, NULL);
}

static bool parse_struct_members(parser_t* parser, ast_node_t* type)
{
        DEBUG("parser: Parsing struct members...");

        while (parser->token.kind != TK_RCURLY) {
                ast_node_t* member;

                member = parse_storage(parser, type);
                if (member == NULL) {
                        return false;
                }

                if (parser->token.kind != TK_SEMICOLON) {
                        error(&parser->token, "Expected \";\"\n");
                        delete_nodes(member);
                        return false;
                }

                member->kind = NK_STRUCT_MEMBER;
                push_node(member, NULL);
                next_token(parser);
        }

        next_token(parser);
        return true;
}

static ast_node_t* parse_struct_declaration(parser_t* parser, ast_node_t* type)
{
        type->kind = NK_STRUCT;

        DEBUG("parser: Parsing struct declaration...");

        if (next_token(parser)->kind != TK_LCURLY) {
                error(&parser->token, "Expected \"{\" after \"struct\"\n");
                delete_nodes(type);
                return NULL;
        }

        /* Parse struct members */
        if (next_token(parser)->kind != TK_RCURLY) {
                if (!parse_struct_members(parser, type)) {
                        delete_nodes(type);
                        return NULL;
                }
        } else {
                next_token(parser);
        }

        push_node(type, NULL);
        return type;
}

ast_node_t* parse_type_declaration(parser_t* parser, bool public)
{
        ast_node_t* type;

        DEBUG("parser: Parsing type declaration...");

        if (next_token(parser)->kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected type name after \"type\"\n");
                return NULL;
        }

        /* Create type and set name */
        type = create_node(parser->types);
        type->flags = NF_NAMED;
        type->name.string = parser->token.pos;
        type->name.length = parser->token.length;
        type->name.hash = parser->token.hash;
        if (public) {
                type->flags |= NF_PUBLIC;
        }

        if (next_token(parser)->kind != TK_COLON) {
                error(&parser->token, "Expected \":\" after type name\n");
                delete_nodes(type);
                return NULL;
        }

        if (next_token(parser)->kind == TK_STRUCT) {
                return parse_struct_declaration(parser, type);
        }

        /* TODO: Implement enums */

        if (parser->token.kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected \"struct\" or type name after \":\"\n");
                delete_nodes(type);
                return NULL;
        }

        /* Parse aliased type */
        if (parse_type_reference(parser, type) == NULL) {
                delete_nodes(type);
                return NULL;
        }

        /* Type aliases must be terminated with a ";" */
        if (parser->token.kind != TK_SEMICOLON) {
                error(&parser->token, "Expected \";\" after type reference\n");
                delete_nodes(type);
                return NULL;
        }

        /* Set alias properties */
        type->kind = NK_TYPE_ALIAS;
        type->bytes = type->type->bytes;

        push_node(type, NULL);
        next_token(parser);
        return type;
}

ast_node_t* parse_type_reference(parser_t* parser, ast_node_t* node)
{
        ast_node_t* type;
        size_t pointer_depth;

        if (parser->token.kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected type name\n");
                return NULL;
        }

        type = find_node(&parser->token, parser->types);
        if (type == NULL) {
                error(&parser->token, "\"%.*s\" does not exist or is not a type\n", parser->token.length, parser->token.pos);
                return NULL;
        }

        /* Find pointer depth */
        pointer_depth = 0;
        while (next_token(parser)->kind == TK_STAR) {
                pointer_depth++;
        }

        node->type = type;
        node->pointer_depth = pointer_depth;
        return type;
}

ast_node_t* init_types(void)
{
        ast_node_t* types;

        DEBUG("parser: Initializing types...");

        types = create_node(NULL);

        create_builtin_type(types, "uint", 8, NF_NONE);
        create_builtin_type(types, "char", 1, NF_NONE);

        return types;
}
