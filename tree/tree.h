#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../diff/dsl.h"
#include "../Include_file/error/error.h"
#include "../Include_file/utilities/utils.h"

#define BLACK_COLOR "\"#000000\""
#define WHITE_COLOR "\"#FFFFFF\""
#define DARK_GREEN_COLOR "\"#006400\""
#define BLUE_COLOR "\"#00BFFF\""
#define PURPLE_COLOR "\"#8B00FF\""
#define RED_COLOR "\"#ff0000\""
#define LIGHT_GREEN_COLOR "\"#ccff99\""
#define BACK_GROUND_COLOR "\"#696969\""

#define FONTNAME "\"Times-New-Roman\""

#ifdef DEBUG
    #define DEBUG_TREE

    #define tree_log(tree, code_error)                                          \
    {                                                                           \
        tree_dump_text(tree, code_error, __FILE__, __func__, __LINE__);         \
        tree_dump_graph_viz(tree, code_error, __FILE__, __func__, __LINE__);    \
    }

    #define assert_tree(tree)               \
    {                                       \
        tree_verificator(tree, code_error); \
        if (*code_error != ERR_NO)          \
        {                                   \
            tree_log(tree, code_error)      \
        }                                   \
    }
#else
    #define tree_log(...)
    #define assert_tree(...)
#endif

enum op_command {
    END_EXPR      = 0,
    ADD           = 1,
    SUB           = 2,
    MUL           = 3,
    DIV           = 4,
    POW           = 5,
    SIN           = 6,
    COS           = 7,
    FLOOR         = 8,
    DIFF          = 9,
    SQRT          = 10,
    LN            = 11,
    IF            = 12,
    ELSE          = 13,
    WHILE         = 14,
    AND           = 15,
    OR            = 16,
    AE            = 17,
    BE            = 18,
    EQUAL         = 19,
    NE            = 20,
    ASSIG         = 21,
    ABOVE         = 22,
    LESS          = 23,
    NOT           = 24,
    PRINT         = 25,
    INPUT         = 26,
    BREAK         = 27,
    CONTINUE      = 28,
    RETURN        = 29,
    OPEN_BRACKET  = 30,
    CLOSE_BRACKET = 31,
    OPEN_BRACE    = 32,
    CLOSE_BRACE   = 33,
    COMMA         = 34,
    FUNC          = 35,

    OP_NO         = 36
};

enum types {
    DEF_TYPE,
    NUM,
    IDENT,
    IDENT_FUNC,
    CALL_FUNC,
    OP
};

const bool LEFT  = false;
const bool RIGHT = true;

typedef double element;

union Data {
    element value;
    op_command types_op;
    int ident;
};

typedef struct Node {
    Node *left   = NULL;
    Node *right  = NULL;
    Node *parent = NULL;

    types type = DEF_TYPE;
    Data data = {};
} Node;

typedef struct Token {
    types type = DEF_TYPE;
    Data data  = {};
} Token;

typedef struct {
    char *ident  = NULL;
    int   number = 0;
    types type   = DEF_TYPE;
} PairsNumIdent;

typedef struct {
    size_t n_ident      = 0;
    size_t n_ident_func = 0;

    PairsNumIdent *pairs_num_ident = NULL;
} TableName;

typedef struct {
    const char *fp_name_expr       = NULL;
    FILE *fp_expr                  = NULL;

    const char *fp_name_tree       = NULL;
    FILE *fp_tree                  = NULL;

    const char *fp_name_table_name = NULL;
    FILE *fp_table_name            = NULL;

    size_t size_file = 0;

    char *buf= NULL;
} Info;

typedef struct {
    Node *root = NULL;

    Token *token   = NULL;
    size_t n_token = 0;

    bool is_init = false;

    Info info = {};
    TableName table_name = {};
} Tree;

int create_tree (Tree *tree, int argc, char *argv[], int *code_error);

Node *create_node_num (element value, Node *left, Node *right, Node *parent, int *code_error);

Node *create_node_op (op_command types_op, Node *left, Node *right, Node *parent, int *code_error);

Node *create_node_ident (int ident, Node *left, Node *right, Node *parent, int *code_error);

Node *create_node (types type, Node *left, Node *right, Node *parent, int *code_error);

Node *set_parent (Node *node, Node *parent);

int delete_node (Node *node);

Node *copy_tree (Node *node, Node *parent, int *code_error);

void print_tree (Node *node, FILE *stream, int *code_error);

void print_table_name (Tree *tree, int *code_error);

int destroy_tree (Tree *tree, int *code_error);

#ifdef DEBUG
    void tree_verificator (Tree *tree, int *code_error);

    void node_verificator (Node *node, int *code_error);

    void tree_dump_text (Tree *tree, int *code_error,
                         const char *file_err, const char *func_err,
                         const int line_err);

    void tree_dump_graph_viz (Tree *tree, int *code_error,
                              const char *file_err, const char *func_err,
                              const int line_err);
#endif

#endif // TREE_H
