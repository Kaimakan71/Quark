/*
 * Parses values.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <error.h>
#include <debug.h>
#include <parser/value.h>

ast_node_t* parse_value(parser_t* parser, ast_node_t* parent)
{
        /* TODO: Support more kinds of values */

        if (parser->token.kind == TK_NUMBER) {
                ast_node_t* number;

                number = create_node(parent);
                number->kind = NK_NUMBER;
                number->value = parser->token.value;
                push_node(number, NULL);

                next_token(parser);
                return number;
        }

        error(&parser->token, "Expected number\n");
        return NULL;
}
