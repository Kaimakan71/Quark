/*
 * Parses procedure declarations.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#ifndef _PARSER_PROCEDURE_H
#define _PARSER_PROCEDURE_H

#include <stdbool.h>
#include <parser.h>

ast_node_t* parse_procedure(parser_t* parser, bool public);

#endif /* !_PARSER_PROCEDURE_H */
