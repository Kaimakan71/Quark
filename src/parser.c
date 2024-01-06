/*
 * Analyzes tokens to create an AST tree.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
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
static ast_node_t* root;

static ast_node_t* create_node(ast_node_t* parent)
{
        ast_node_t* node;

        node = malloc(sizeof(ast_node_t));
        node->parent = parent;
        node->first_child = NULL;
        node->last_child = NULL;
        node->next = NULL;

        return node;
}

static void push_node(ast_node_t* node)
{
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

static void free_nodes(ast_node_t* parent)
{
        ast_node_t* node;
        ast_node_t* next;

        node = parent;
        node->parent = NULL;
        while (1) {
                if (node->first_child != NULL) {
                        next = node->first_child;
                } else if (node->next != NULL) {
                        next = node->next;
                } else if (node->parent != NULL) {
                        next = node->parent;
                } else {
                        /* Reached top, exit now */
                        free(node);
                        break;
                }

                free(node);
                node = next;
        }
}

static node_type_t parse_operator(token_t* token)
{
        switch (token->type) {
        case TT_PLUS:
                return NT_ADD;
        case TT_MINUS:
                return NT_SUBTRACT;
        case TT_STAR:
                return NT_MULTIPLY;
        case TT_SLASH:
                return NT_DIVIDE;
        default:
                return NT_UNKNOWN;
        }
}

static ast_node_t* parse_lvalue(ast_node_t* operation, token_t* token)
{
        ast_node_t* left;

        /* TODO: Allow identifiers as operands */
        if (token->type == TT_NUMBER) {
                left = create_node(operation);
                left->type = NT_NUMBER;
                left->value = token->value;
                push_node(left);
                lexer_next(token);
                return left;
        }

        /* Operator for compound operations (like 2+3/5) */
        operation->type = parse_operator(token);
        if (operation->type == NT_UNKNOWN) {
                return NULL;
        }

        /* Apply order of operations */
        /* TODO: Use a table to find operator precedence? */
        if ((operation->type == NT_MULTIPLY || operation->type == NT_DIVIDE) && (operation->prev->type == NT_ADD || operation->prev->type == NT_SUBTRACT)) {
                operation->parent = operation->prev;
        }
        left = pop_node(operation->parent);
        left->parent = operation;
        lexer_next(token);
        push_node(left);

        return left;
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

                        /* End of expression */
                        if (left == NULL) {
                                break;
                        }

                        continue;
                }

                /* Find rvalue */
                if (right == NULL && operation->type != NT_UNKNOWN) {
                        /* TODO: Allow identifiers as operands */
                        if (token->type != TT_NUMBER) {
                                error(token, "Expected right-hand operand after operator\n");
                                break;
                        }

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

                /* Find operator */
                if (left != NULL && right == NULL) {
                        operation->type = parse_operator(token);
                        if (operation->type == NT_UNKNOWN) {
                                error(token, "Expected operator after left-hand operand\n");
                                break;
                        }

                        lexer_next(token);
                        continue;
                }
        }

        /* Free unused memory */
        free_nodes(operation);
}

static void parse_variable(ast_node_t* parent, token_t* token)
{
        ast_node_t* variable;

        variable = create_node(parent);
        variable->type = NT_VARIABLE;

        /* TODO: Variable types */
        lexer_next(token);
        if (token->type != TT_IDENTIFIER) {
                error(token, "Expected identifier after type name\n");
                free_nodes(variable);
                return;
        }

        variable->name.string = token->pos;
        variable->name.length = token->length;
        variable->name.hash = token->hash;

        /* Variable declaration (no initialization) */
        lexer_next(token);
        if (token->type == TT_SEMICOLON) {
                push_node(variable);
                lexer_next(token);
                return;
        }

        if (token->type != TT_EQUALS) {
                error(token, "Expected \";\" or \"=\" after variable name\n");
                free_nodes(variable);
                return;
        }

        /* Variable definition (initialization) */
        lexer_next(token);
        parse_expression(variable, token);
        if (token->type != TT_SEMICOLON) {
                error(token, "Expected \";\" after expression\n");
                free_nodes(variable);
                return;
        }

        push_node(variable);
        lexer_next(token);
}

void parse_function(ast_node_t* parent, token_t* token)
{
        ast_node_t* function;

        function = create_node(parent);
        function->type = NT_FUNCTION;

        lexer_next(token);
        if (token->type == TT_IDENTIFIER) {
                function->name.string = token->pos;
                function->name.length = token->length;
                function->name.hash = token->hash;
        }

        lexer_next(token);
        if (token->type != TT_LPAREN) {
                error(token, "Expected \"(\" after function name\n");
                free_nodes(function);
                return;
        }

        /* TODO: Function arguments */
        lexer_next(token);
        if (token->type != TT_RPAREN) {
                error(token, "Expected \")\" after \"(\"\n");
                free_nodes(function);
                return;
        }

        /* TODO: Function defintions (bodies) */
        lexer_next(token);
        if (token->type != TT_SEMICOLON) {
                error(token, "Expected \";\" after \")\"\n");
                free_nodes(function);
                return;
        }

        push_node(function);
        lexer_next(token);
}

static void parse_function_call(ast_node_t* parent, token_t* token)
{
        ast_node_t* call;

        call = create_node(parent);
        call->type = NT_FUNCTION_CALL;
        call->name.string = token->pos;
        call->name.length = token->length;
        call->name.hash = token->hash;

        lexer_next(token);
        if (token->type != TT_LPAREN) {
                error(token, "Expected \"(\" after function name\n");
                free_nodes(call);
                return;
        }

        /* TODO: Function arguments */
        lexer_next(token);
        if (token->type != TT_RPAREN) {
                error(token, "Expected \")\" after \"(\"\n");
                free_nodes(call);
                return;
        }

        lexer_next(token);
        if (token->type != TT_SEMICOLON) {
                error(token, "Expected \";\" after \")\"\n");
                free_nodes(call);
                return;
        }

        push_node(call);
        lexer_next(token);
}

ast_node_t* parse(char* source)
{
        token_t token;

        root = create_node(NULL);

        lexer_init(source);
        lexer_next(&token);
        while (token.type != TT_EOF) {
                if (token.type == TT_FUNCTION) {
                        parse_function(root, &token);
                        continue;
                }

                if (token.type == TT_IDENTIFIER) {
                        parse_function_call(root, &token);
                        continue;
                }

                if (token.type == TT_UINT) {
                        parse_variable(root, &token);
                        continue;
                }

                if (token.type == TT_UNKNOWN) {
                        error(&token, "Unknown token\n");
                        break;
                }

                error(&token, "Unexpected '%.*s'\n", token.length, token.pos);
                break;
        }

        return root;
}
