/*
 * Parses statements.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _PARSER_STATEMENT_H
#define _PARSER_STATEMENT_H

#include <stdbool.h>
#include "parser.h"

ast_node_t* parse_statement(parser_t* parser, ast_node_t* parent, ast_node_t* procedure);
bool parse_statement_group(parser_t* parser, ast_node_t* parent, ast_node_t* procedure);

#endif /* !_PARSER_STATEMENT_H */
