/*
 * Parses variable declarations.
 * Copyright (c) 2023-2024, Quinn Stephens.
 * Provided under the BSD 3-Clause license.
 */

#ifndef _PARSER_VARIABLE_H
#define _PARSER_VARIABLE_H

#include "parser.h"

ast_node_t* parse_variable_declaration(parser_t* parser, ast_node_t* parent, token_t* type_name);
ast_node_t* parse_variable_reference(ast_node_t* parent, token_t* variable_name);
ast_node_t* parse_local_declaration(parser_t* parser, ast_node_t* parent, ast_node_t* procedure, token_t* type_name);

#endif /* !_PARSER_VARIABLE_H */
