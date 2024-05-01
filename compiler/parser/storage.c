/*
 * Parses storage declarations.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <error.h>
#include <debug.h>
#include <parser/type.h>
#include <parser/storage.h>

ast_node_t* parse_storage(parser_t* parser, ast_node_t* parent)
{
        ast_node_t* variable;

        DEBUG("parser: Parsing storage declartation...");

        /* Create variable and parse type */
        variable = create_node(parent);
        variable->flags = NF_NAMED;
        if (parse_type_reference(parser, variable) == NULL) {
                delete_nodes(variable);
                return NULL;
        }

        if (parser->token.kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected name after type\n");
                delete_nodes(variable);
                return NULL;
        }

        /* Prevent redeclaring a variable */
        if (find_node(&parser->token, parent) != NULL) {
                error(&parser->token, "\"%.*s\" has already been declared\n", parser->token.length, parser->token.pos);
                delete_nodes(variable);
                return NULL;
        }

        /* Set variable name */
        variable->name.string = parser->token.pos;
        variable->name.length = parser->token.length;
        variable->name.hash = parser->token.hash;

        next_token(parser);
        return variable;
}

