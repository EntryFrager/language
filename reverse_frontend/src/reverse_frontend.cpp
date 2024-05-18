#ifndef REVERSE_FRONTEND_CPP
#define REVERSE_FRONTEND_CPP

#include "../inc/reverse_frontend.h"

static int offset_begin_line = 0;
static int cur_scope         = 0;
const  int func_scope        = -1;

#define DEF_CMD(num, name) name,

static const char *OP_NAME[] = {
    #include "../../tree/comand.h"

    "CALL_FUNC",
    "no key word"
};

static void  print_funcs             (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_func              (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_params            (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_param             (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_body              (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_node              (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_op                (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_if_else           (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_while             (Node *node, Identificators *idents, FILE *stream, int *code_error);
static void  print_offset_begin_line (FILE *stream);
static char *get_name_ident          (Identificators *idents, int n_var, int n_scope, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

void print_reverse_frontend (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    fopen_(fp_expr, tree->info.fp_name_expr, "w");

    print_funcs(tree->root, &tree->idents, fp_expr, code_error);

    fclose_(fp_expr);
}

void print_funcs (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(node   != NULL, ERR_PTR);
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    Node *copy_node = node;

    while (copy_node->left != NULL)
    {
        print_func(copy_node->left, idents, stream, code_error);

        fprintf(stream, "\n");

        cur_scope++;

        copy_node = copy_node->right;
    }
}

void print_func (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(node   != NULL, ERR_PTR);
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    fprintf(stream, "%s %s", OP_NAME[FUNC], get_name_ident(idents, node->data.ident, -1, code_error));

    print_params(node->left, idents, stream, code_error);

    fprintf(stream, "\n");

    print_body(node->right, idents, stream, code_error);

    offset_begin_line = 0;
}

void print_params (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    Node *params_node = node;

    fprintf(stream, "(");

    while(params_node != NULL)
    {
        print_param(params_node, idents, stream, code_error);

        params_node = params_node->left;
    }

    fprintf(stream, ")");
}

void print_param (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(node   != NULL, ERR_PTR);
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    fprintf(stream, "%s", get_name_ident(idents, node->data.ident, cur_scope, code_error));

    if (node->left != NULL)
    {
        fprintf(stream, ", ");
    }
}

void print_body (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    Node *body_node = node;

    print_offset_begin_line(stream);

    fprintf(stream, "%s\n", OP_NAME[OPEN_BRACE]);

    offset_begin_line += 1;

    while (body_node != NULL)
    {
        print_offset_begin_line(stream);

        print_node(body_node->left, idents, stream, code_error);

        if (body_node->left->type != OP || (body_node->left->type == OP && body_node->left->data.types_op != IF &&
            body_node->left->data.types_op != WHILE))
        {
            fprintf(stream, ";\n");
        }

        body_node = body_node->right;
    }

    offset_begin_line -= 1;

    print_offset_begin_line(stream);

    fprintf(stream, "%s\n", OP_NAME[CLOSE_BRACE]);
}

void print_node (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    IS_NODE_PTR_NULL();

    switch (node->type)
    {
        case (NUM):
        {
            fprintf(stream, "%lg", node->data.value);
            break;
        }
        case (IDENT):
        {
            fprintf(stream, "%s", get_name_ident(idents, node->data.ident, cur_scope, code_error));
            break;
        }
        case (CALL_FUNC):
        {
            fprintf(stream, "%s", get_name_ident(idents, node->data.ident, -1, code_error));
            print_params(node->left, idents, stream, code_error);
            break;
        }
        case (OP):
        {
            print_op(node, idents, stream, code_error);
            break;
        }
        default:
        {
            break;
        }
    }
}

void print_op (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(node   != NULL, ERR_PTR);
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    switch (node->data.types_op)
    {
        case (ADD):
        case (SUB):
        case (MUL):
        case (DIV):
        case (POW):
        case (ASSIG):
        case (AND):
        case (OR):
        case (AE):
        case (BE):
        case (EQUAL):
        case (NE):
        case (ABOVE):
        case (LESS):
        {
            print_node(node->left, idents, stream, code_error);
            fprintf(stream, " %s ", OP_NAME[node->data.types_op]);
            print_node(node->right, idents, stream, code_error);

            break;
        }
        case (SIN):
        case (COS):
        case (FLOOR):
        case (SQRT):
        case (LN):
        {
            fprintf(stream, "%s(", OP_NAME[node->data.types_op]);
            print_node(node->left, idents, stream, code_error);
            fprintf(stream, ")");

            break;
        }
        case (BREAK):
        case (CONTINUE):
        case (ABORT):
        case (INPUT):
        case (PRINT):
        {
            fprintf(stream, "%s", OP_NAME[node->data.types_op]);
            print_params(node->left, idents, stream, code_error);

            break;
        }
        case (RETURN):
        {
            fprintf(stream, "%s ", OP_NAME[node->data.types_op]);
            print_node(node->left, idents, stream, code_error);

            break;
        }
        case (WHILE):
        {
            print_while(node, idents, stream, code_error);
            break;
        }
        case (IF):
        {
            print_if_else(node, idents, stream, code_error);
            break;
        }
        default:
        {
            fprintf(stream, "%s", OP_NAME[node->data.types_op]);
            break;
        }
    }
}

void print_if_else (Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    if (node->data.types_op != IF)
    {
        print_offset_begin_line(stream);
    }

    fprintf(stream, "%s", OP_NAME[node->data.types_op]);

    if (node->data.types_op != ELSE)
    {
        fprintf(stream, " (");

        print_node(node->left->left, idents, stream, code_error);

        fprintf(stream, ")");
    }

    fprintf(stream, "\n");

    print_body(node->left->right, idents, stream, code_error);

    print_if_else(node->right, idents, stream, code_error);
}

void print_while(Node *node, Identificators *idents, FILE *stream, int *code_error)
{
    my_assert(node   != NULL, ERR_PTR);
    my_assert(idents != NULL, ERR_PTR);
    my_assert(stream != NULL, ERR_PTR);

    fprintf(stream, "%s (", OP_NAME[node->data.types_op]);

    print_node(node->left, idents, stream, code_error);

    fprintf(stream, ")");

    offset_begin_line += 1;

    print_body(node->right, idents, stream, code_error);
}

void print_offset_begin_line (FILE *stream)
{
    for (int i = 0; i < offset_begin_line; i++)
    {
        fprintf(stream, "\t");
    }
}

char *get_name_ident (Identificators *idents, int n_var, int n_scope, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);

    printf("%d %d\n", n_scope, n_var);

    for (size_t i = 0; i < idents->n_ident; i++)
    {
        if (idents->ident[i].n_var == n_var && idents->ident[i].n_scope == n_scope)
        {
            printf("%s\n", idents->ident[i].name_var);
            return idents->ident[i].name_var;
        }
    }

    return NULL;
}

#pragma GCC diagnostic pop

#endif
