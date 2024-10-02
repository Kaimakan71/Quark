/*
 * Parses variable declarations.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include "log.h"
#include "parser/type.h"
#include "parser/value.h"
#include "parser/variable.h"

ast_node_t* parse_variable_declaration(parser_t* parser, ast_node_t* parent, token_t* type_name)
{
        ast_node_t* variable;

        debug("Parsing variable declartation...");

        /* Create variable and parse type */
        variable = create_node(parent);
        variable->flags = NF_NAMED;
        if (parse_type_reference(parser, variable, type_name) == NULL) {
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

ast_node_t* parse_variable_reference(ast_node_t* parent, token_t* variable_name)
{
        ast_node_t* reference;
        ast_node_t* variable;

        variable = find_node(variable_name, parent);
        if (variable == NULL || (variable->kind != NK_LOCAL_VARIABLE && variable->kind != NK_PARAMETER)) {
                error(variable_name, "\"%.*s\" does not exist or is not a variable\n", variable_name->length, variable_name->pos);
                return NULL;
        }

        /* TODO: Struct and enum member references */

        reference = create_node(parent);
        reference->kind = NK_VARIABLE_REFERENCE;
        reference->variable = variable;

        push_node(reference, NULL);
        return reference;
}

ast_node_t* parse_local_declaration(parser_t* parser, ast_node_t* parent, ast_node_t* procedure, token_t* type_name)
{
        ast_node_t* variable;

        debug("Parsing local variable declaration...");

        variable = parse_variable_declaration(parser, parent, type_name);
        if (variable == NULL) {
                return NULL;
        }

        if (parser->token.kind == TK_EQUALS) {
                next_token(parser);
                if (parse_value(parser, variable) == NULL) {
                        delete_nodes(variable);
                        return NULL;
                }
        }

        if (parser->token.kind != TK_SEMICOLON) {
                error(&parser->token, "Expected \";\" after variable declaration\n");
                delete_nodes(variable);
                return NULL;
        }

        variable->kind = NK_LOCAL_VARIABLE;
        variable->local_offset = procedure->local_size;
        procedure->local_size += variable->bytes;

        push_node(variable, NULL);
        next_token(parser);
        return variable;
}
