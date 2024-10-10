/*
 * Parses procedure declarations.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#include "log.h"
#include "parser.h"
#include "parser/procedure.h"
#include "parser/statement.h"
#include "parser/variable.h"
#include "parser/type.h"
#include "parser/value.h"

static bool parse_parameters(parser_t* parser, ast_node_t* parent)
{
        debug("Parsing parameters...");

        while (parser->token.kind != TK_RPAREN) {
                ast_node_t* parameter;

                parameter = parse_variable_declaration(parser, parent, NULL);
                if (parameter == NULL) {
                        return false;
                }

                /* Push parameter to parameter list */
                parameter->kind = NK_PARAMETER;

                /* TODO: Use &parent->params instead of NULL */
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

ast_node_t* parse_proc_declaration(parser_t* parser)
{
        ast_node_t* procedure;

        debug("Parsing procedure declaration...");

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
                if (parse_type_reference(parser, procedure, NULL) == NULL) {
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
                procedure->flags |= NF_DEFINITION;
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

ast_node_t* parse_proc_call(parser_t* parser, ast_node_t* parent, token_t* callee_name)
{
        ast_node_t* callee;
        ast_node_t* call;

        debug("Parsing procedure call...");

        callee = find_node(callee_name, parent);
        if (callee == NULL) {
                error(callee_name, "\"%.*s\" does not exist\n", callee_name->length, callee_name->pos);
                return NULL;
        }

        /* Create call */
        call = create_node(parent);
        call->kind = NK_CALL;
        call->callee = callee;

        next_token(parser);
        while (parser->token.kind != TK_RPAREN) {
                if (parse_value(parser, call) == NULL) {
                        delete_nodes(call);
                        return NULL;
                }

                if (parser->token.kind == TK_COMMA && next_token(parser)->kind == TK_RPAREN) {
                        warn(&parser->token, "Extra \",\" after arguments\n");
                }
        }

        push_node(call, NULL);
        next_token(parser);
        return call;
}
