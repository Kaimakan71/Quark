/*
 * Parses procedure declarations.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <error.h>
#include <debug.h>
#include <parser.h>
#include <parser/type.h>
#include <parser/statement.h>
#include <parser/procedure.h>
#include <parser/storage.h>

static bool parse_parameters(parser_t* parser, ast_node_t* parent)
{
        DEBUG("parser: Parsing parameters...");

        while (parser->token.kind != TK_RPAREN) {
                ast_node_t* parameter;

                parameter = parse_storage(parser, parent);
                if (parameter == NULL) {
                        return false;
                }

                /* Push parameter to parameter list */
                parameter->kind = NK_PARAMETER;

                /* TODO: Use &parent->parameters instead of NULL */
                push_node(parameter, NULL);

                /* Parameters are seperated by "," */
                if (parser->token.kind != TK_COMMA && parser->token.kind != TK_RPAREN) {
                        error(&parser->token, "Expected \",\" or \")\" after parameter definition\n");
                        return false;
                }
        }

        next_token(parser);
        return true;
}

ast_node_t* parse_procedure(parser_t* parser, bool public)
{
        ast_node_t* procedure;

        DEBUG("parser: Parsing procedure declaration...");

        if (next_token(parser)->kind != TK_IDENTIFIER) {
                error(&parser->token, "Expected procedure name after \"proc\"\n");
                return NULL;
        }

        /* Create procedure and set name */
        procedure = create_node(parser->procedures);
        procedure->kind = NK_PROCEDURE;
        procedure->flags = NF_NAMED;
        procedure->name.string = parser->token.pos;
        procedure->name.length = parser->token.length;
        procedure->name.hash = parser->token.hash;
        if (public) {
                procedure->flags |= NF_PUBLIC;
        }

        if (next_token(parser)->kind != TK_LPAREN) {
                error(&parser->token, "Expected \"(\" after procedure name\n");
                delete_nodes(procedure);
                return NULL;
        }

        /* Parse parameters, if any */
        if (next_token(parser)->kind != TK_RPAREN) {
                if (!parse_parameters(parser, procedure)) {
                        delete_nodes(procedure);
                        return NULL;
                }
        } else {
                next_token(parser);
        }

        /* Parse return type, if any */
        if (parser->token.kind == TK_ARROW) {
                next_token(parser);
                if (parse_type_reference(parser, procedure) == NULL) {
                        delete_nodes(procedure);
                        return NULL;
                }
        }

        /* Procedure declarations are terminated with a ";" */
        if (parser->token.kind == TK_SEMICOLON) {
                push_node(procedure, NULL);
                next_token(parser);
                return procedure;
        }

        /* Procedure body must start with a "{" */
        if (parser->token.kind != TK_LCURLY) {
                error(&parser->token, "Expected \";\" or \"{\" after \")\"\n");
                delete_nodes(procedure);
                return NULL;
        }

        /* Parse body, if any */
        if (next_token(parser)->kind != TK_RCURLY) {
                if (!parse_statement_group(parser, procedure, procedure)) {
                        delete_nodes(procedure);
                        return NULL;
                }
        } else {
                next_token(parser);
        }

        push_node(procedure, NULL);
        return procedure;
}
