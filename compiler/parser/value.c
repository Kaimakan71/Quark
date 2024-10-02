/*
 * Parses values.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include "log.h"
#include "parser/procedure.h"
#include "parser/value.h"
#include "parser/variable.h"
#include "string.h"

ast_node_t* parse_value(parser_t* parser, ast_node_t* parent)
{
        token_t name;

        if (parser->token.kind == TK_NUMBER) {
                ast_node_t* number;

                number = create_node(parent);
                number->kind = NK_NUMBER;
                number->value = parser->token.value;
                push_node(number, NULL);

                next_token(parser);
                return number;
        }

        if (parser->token.kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected value\n");
                return NULL;
        }

        /* Save name */
        memcpy(&name, &parser->token, sizeof(token_t));
        next_token(parser);

        /* TODO: Structs will eventually have methods like console.print() */
        if (parser->token.kind == TK_LPAREN) {
                return parse_proc_call(parser, parent, &name);
        }

        return parse_variable_reference(parent, &name);
}
