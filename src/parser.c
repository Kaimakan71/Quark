/*
 * Analyzes tokens to create an AST tree.
 * Copyright (c) 2023-2023, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quark/error.h"
#include "quark/lexer.h"
#include "quark/parser.h"

static char* filename;
static char* input;
static ast_node_t root_node;

static ast_node_t* create_node(ast_node_t* parent)
{
        ast_node_t* node;

        node = malloc(sizeof(ast_node_t));
        node->parent = parent;
        node->first_child = NULL;
        node->last_child = NULL;
        node->prev = parent->last_child;

        return node;
}

static void push_node(ast_node_t* node)
{
        node->next = NULL;

        /* Add the node to the doubly linked list */
        if (node->parent->first_child == NULL) {
                node->prev = NULL;
                node->parent->first_child = node;
        } else {
                node->prev = node->parent->last_child;
                node->parent->last_child->next = node;
        }
        node->parent->last_child = node;
}

static ast_node_t* pop_node(ast_node_t* parent)
{
        ast_node_t* node;

        node = parent->last_child;

        /* Remove the node from the doubly linked list*/
        parent->last_child = node->prev;
        if (node == parent->first_child) {
                parent->first_child = NULL;
        }
        if (parent->last_child != NULL) {
                parent->last_child->next = NULL;
        }

        node->prev = NULL;
        return node;
}

static ast_node_t* parse_lvalue(ast_node_t* operation, token_t* token)
{
        ast_node_t* left;

        /* Find value or operator */
        switch (token->type) {
        case TT_NUMBER:
                left = create_node(operation);
                left->type = NT_NUMBER;
                left->value = token->value;
                push_node(left);
                lexer_next(token);
                return left;
        case TT_PLUS:
                operation->type = NT_ADD;
                break;
        case TT_MINUS:
                operation->type = NT_SUBTRACT;
                break;
        case TT_STAR:
                operation->type = NT_MULTIPLY;
                break;
        case TT_SLASH:
                operation->type = NT_DIVIDE;
                break;
        default:
                return NULL;
        }

        /* Apply order of operations */
        if ((operation->type == NT_MULTIPLY || operation->type == NT_DIVIDE) && (operation->prev->type == NT_ADD || operation->prev->type == NT_SUBTRACT)) {
                operation->parent = operation->prev;
                left = pop_node(operation->parent);
                left->parent = operation;
                push_node(left);
                lexer_next(token);
                return left;
        } else {
                left = pop_node(operation->parent);
                left->parent = operation;
                push_node(left);
                lexer_next(token);
                return left;
        }
}

static void parse_expression(ast_node_t* parent, token_t* token)
{
        ast_node_t* operation;
        ast_node_t* left;
        ast_node_t* right;

        operation = create_node(parent);
        operation->type = NT_UNKNOWN;
        left = NULL;
        right = NULL;
        while (1) {
                /* Find lvalue */
                if (left == NULL) {
                        left = parse_lvalue(operation, token);
                        if (left == NULL) {
                                return;
                        }

                        continue;
                }

                /* Find rvalue */
                if (right == NULL && operation->type != NT_UNKNOWN) {
                        if (token->type == TT_NUMBER) {
                                right = create_node(operation);
                                right->type = NT_NUMBER;
                                right->value = token->value;
                                push_node(right);
                                lexer_next(token);

                                push_node(operation);
                                operation = create_node(operation->parent);
                                operation->type = NT_UNKNOWN;
                                left = NULL;
                                right = NULL;
                                continue;
                        }

                        error(token, "Expected right-hand operand after operator\n");
                        break;
                }

                /* Find operator */
                if (left != NULL && right == NULL) {
                        if (token->type == TT_PLUS) {
                                operation->type = NT_ADD;
                                lexer_next(token);
                                continue;
                        }

                        if (token->type == TT_MINUS) {
                                operation->type = NT_SUBTRACT;
                                lexer_next(token);
                                continue;
                        }

                        if (token->type == TT_STAR) {
                                operation->type = NT_MULTIPLY;
                                lexer_next(token);
                                continue;
                        }

                        if (token->type == TT_SLASH) {
                                operation->type = NT_DIVIDE;
                                lexer_next(token);
                                continue;
                        }

                        error(token, "Expected operator after left-hand operand\n");
                        break;
                }
        }
}

void parse_variable(token_t* token)
{
        ast_node_t* variable;

        variable = create_node(&root_node);
        variable->type = NT_VARIABLE;

        lexer_next(token);
        if (token->type == TT_IDENTIFIER) {
                variable->name.string = token->pos;
                variable->name.length = token->length;
                variable->name.hash = token->hash;
        }

        lexer_next(token);
        if (token->type == TT_SEMICOLON) {
                push_node(variable);
                lexer_next(token);
                return;
        }

        if (token->type == TT_EQUALS) {
                lexer_next(token);
                parse_expression(variable, token);
                if (token->type != TT_SEMICOLON) {
                        error(token, "Expected semicolon after variable initialization\n");
                        return;
                }

                push_node(variable);
                lexer_next(token);
                return;
        }

        error(token, "Expected semicolon or expression after \"=\"\n");
}

ast_node_t* parse(char* source)
{
        token_t token;

        lexer_init(source);
        lexer_next(&token);
        memset(&root_node, 0, sizeof(ast_node_t));
        while (token.type != TT_EOF) {
                if (token.type == TT_UINT) {
                        parse_variable(&token);
                        continue;
                }

                if (token.type == TT_UNKNOWN) {
                        error(&token, "Unknown token\n");
                        break;
                }

                error(&token, "Unexpected '%.*s'\n", token.length, token.pos);
                break;
        }

        return &root_node;
}
