/*
 * Parses storage declarations.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_STORAGE_H
#define _PARSER_STORAGE_H

#include <parser.h>
#include <parser/ast.h>

ast_node_t* parse_storage_declaration(parser_t* parser, ast_node_t* parent);
ast_node_t* parse_variable_reference(parser_t* parser, ast_node_t* parent);
ast_node_t* parse_local_declaration(parser_t* parser, ast_node_t* parent, ast_node_t* procedure);

#endif /* !_PARSER_STORAGE_H */
