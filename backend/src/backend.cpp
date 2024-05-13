#ifndef BACKEND_CPP
#define BACKEND_CPP

#include "../inc/backend.h"

static int if_label    = 0;
static int while_label = 0;

static bool is_ret = 0;

static void asm_funcs       (Tree *tree, Node *node, int *code_error);
static void asm_func        (Tree *tree, Node *node, int *code_error);
static void asm_body        (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_node        (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_call_func   (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_params      (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_op          (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_if_else     (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_while       (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_condition   (Tree *tree, Node *node, ScopeTableName *cur_table, op_command mode, int n_label, int *code_error);
static void asm_compare     (Tree *tree, Node *node, op_command mode, int label, int *code_error);
static void asm_opp_compare (Tree *tree, Node *node, op_command mode, int label, int *code_error);
static void asm_print       (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);
static void asm_input       (Tree *tree, Node *node, int *code_error);
static void asm_return      (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wswitch-enum"

#define PRINT_ASM(str) fprintf(tree->info.fp_asm, str);
#define PRINT_ASM_PARAM(str, ...) fprintf(tree->info.fp_asm, str, __VA_ARGS__);

void print_asm_code (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    PRINT_ASM("push 0\n"
              "pop rbx\n"
              "call main\n"
              "hlt\n");

    asm_funcs(tree, tree->root, code_error);

    PRINT_ASM("hlt\n");
}

void asm_funcs (Tree *tree, Node *node, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    Node *temp_node = node;

    while (temp_node != NULL)
    {
        asm_func(tree, temp_node->left, code_error);

        temp_node = temp_node->right;
    }
}

void asm_func (Tree *tree, Node *node, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    PRINT_ASM_PARAM("%s:\n", tree->idents.ident[node->data.ident].name_var);

    ScopeTableName *func_table_name = NULL;

    for (size_t i = 0; i < tree->table_name.n_scope; i++)
    {
        if (node->data.ident == tree->table_name.scope_table_name[i]->code_table_name)
        {
            func_table_name = tree->table_name.scope_table_name[i];
        }
    }

    Node *params = node->left;

    asm_body(tree, node->right, func_table_name, code_error);

    if (!is_ret)
    {
        PRINT_ASM("\t\tret\n");
    }

    is_ret = false;
}

void asm_body (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    Node *temp_node = node;

    while (temp_node != NULL)
    {
        asm_node(tree, temp_node->left, cur_table,  code_error);

        temp_node = temp_node->right;
    }
}

void asm_node (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    switch (node->type)
    {
        case (NUM):
        {
            PRINT_ASM_PARAM("\t\tpush %lg\n", node->data.value);
            break;
        }
        case (IDENT):
        {
            PRINT_ASM_PARAM("\t\tpush [rbx + %d]\n", node->data.ident);
            break;
        }
        case (CALL_FUNC):
        {
            asm_call_func(tree, node, cur_table, code_error);
            break;
        }
        case (PARAM):
        {
            PRINT_ASM_PARAM("\t\tpush [rbx + %d]\n", node->data.ident);
            break;
        }
        case (OP):
        {
            asm_op(tree, node, cur_table, code_error);
            break;
        }
        default:
        {
            break;
        }
    }
}

void asm_call_func (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);
    my_assert(node      != NULL, ERR_PTR);

    asm_params(tree, node->left, cur_table, code_error);

    PRINT_ASM_PARAM("\t\tpush rbx\n"
                    "\t\tpush rbx\n"
                    "\t\tpush %ld\n"
                    "\t\tadd\n"
                    "\t\tpop rbx\n", cur_table->n_elem);

    PRINT_ASM_PARAM("\t\tcall %s\n", tree->idents.ident[node->data.ident].name_var);

    PRINT_ASM("\t\tpop rbx\n"
              "\t\tpush rax\n");
}

void asm_params (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    Node *temp_node = node;

    for (int i = 0; temp_node != NULL; i++)
    {
        asm_node(tree, temp_node, cur_table, code_error);

        PRINT_ASM_PARAM("\t\tpop [rbx + %ld]\n", cur_table->n_elem + i + 1);

        temp_node = temp_node->left;
    }
}

#define EXPR_ASM(str)                                   \
    asm_node(tree, node->left,  cur_table, code_error); \
    asm_node(tree, node->right, cur_table, code_error); \
    PRINT_ASM(str);

#define EXPR_ASM_SIDE(str)                              \
    asm_node(tree, node->left, cur_table, code_error);  \
    PRINT_ASM(str);

void asm_op (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    switch (node->data.types_op)
    {
        case (END_EXPR):
        {
            asm_node(tree, node->left, cur_table, code_error);
            asm_node(tree, node->right, cur_table, code_error);
            break;
        }
        case (ADD):
        {
            EXPR_ASM("\t\tadd\n");
            break;
        }
        case (SUB):
        {
            EXPR_ASM("\t\tsub\n");
            break;
        }
        case (MUL):
        {
            EXPR_ASM("\t\tmul\n");
            break;
        }
        case (DIV):
        {
            EXPR_ASM("\t\tdiv\n");
            break;
        }
        case (POW):
        {
            EXPR_ASM("\t\tpow\n");
            break;
        }
        case (SIN):
        {
            EXPR_ASM_SIDE("\t\tsin\n");
            break;
        }
        case (COS):
        {
            EXPR_ASM_SIDE("\t\tcos\n");
            break;
        }
        case (FLOOR):
        {
            EXPR_ASM_SIDE("\t\tfloor\n");
            break;
        }
        case (SQRT):
        {
            EXPR_ASM_SIDE("\t\tsqrt\n");
            break;
        }
        case (LN):
        {
            EXPR_ASM_SIDE("\t\tln\n");
            break;
        }
        case (ASSIG):
        {
            int item_ident = node->left->data.ident;

            asm_node(tree, node->right, cur_table, code_error);

            PRINT_ASM_PARAM("\t\tpop [rbx + %d]\n", item_ident);

            break;
        }
        case (IF):
        {
            asm_if_else(tree, node, cur_table, code_error);
            return;
        }
        case (WHILE):
        {
            asm_while(tree, node, cur_table, code_error);
            return;
        }
        case (PRINT):
        {
            asm_print(tree, node, cur_table, code_error);
            break;
        }
        case (INPUT):
        {
            asm_input(tree, node, code_error);
            break;
        }
        case (BREAK):
        {
            PRINT_ASM_PARAM("\t\tjmp while_%d\n", while_label - 1);
            break;
        }
        case (CONTINUE):
        {
            PRINT_ASM_PARAM("\t\tjmp while_%d", while_label - 2);
            break;
        }
        case (ABORT):
        {
            PRINT_ASM("\t\thlt\n");
            break;
        }
        case (RETURN):
        {
            asm_return(tree, node, cur_table, code_error);
            break;
        }
        default:
        {
            break;
        }
    }
}

#undef EXPR_ASM
#undef EXPR_ASM_SIDE

void asm_if_else (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(node      != NULL, ERR_PTR);
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    Node *temp_node = node;

    int end_if = if_label++;

    int end_label_body = 0;

    while (temp_node != NULL)
    {
        if (temp_node->data.types_op != ELSE)
        {
            end_label_body = if_label++;
            asm_condition(tree, temp_node->left->left, cur_table, IF, end_label_body, code_error);
        }

        asm_node(tree, temp_node->left->right, cur_table, code_error);

        if (temp_node->data.types_op != ELSE)
        {
            PRINT_ASM_PARAM("\t\tjmp end_if_%d\n", end_if);
            PRINT_ASM_PARAM("\tif_%d:\n", end_label_body)
        }

        temp_node = temp_node->right;
    }

    PRINT_ASM_PARAM("\tend_if_%d:\n", end_if);
}

void asm_while (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(node      != NULL, ERR_PTR);
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    int start_label_body = while_label++;
    int end_label_body   = while_label;

    PRINT_ASM_PARAM("\twhile_%d:\n", start_label_body);

    asm_condition(tree, node->left, cur_table, WHILE, end_label_body, code_error);

    while_label++;

    asm_node(tree, node->right, cur_table, code_error);

    PRINT_ASM_PARAM("\t\tjmp while_%d\n", end_label_body);
    PRINT_ASM_PARAM("\twhile_%d:\n", end_label_body);
}

void asm_condition (Tree *tree, Node *node, ScopeTableName *cur_table, op_command mode, int n_label, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    int cur_label = if_label;

    if (node->type == OP && (node->data.types_op == AND || node->data.types_op == OR))
    {
        asm_condition(tree, node->left, cur_table, mode, n_label, code_error);

        if (node->data.types_op == AND)
        {
            asm_opp_compare(tree, node->left, mode, n_label, code_error);
        }
        else if (node->data.types_op == OR)
        {
            asm_compare(tree, node->left, mode, if_label, code_error);

            cur_label = if_label++;
        }

        asm_condition(tree, node->right, cur_table, mode, n_label, code_error);

        asm_opp_compare(tree, node->right, mode, n_label, code_error);

        if (node->data.types_op == OR)
        {
            PRINT_ASM_PARAM("\tif_%d:\n", cur_label);
        }
    }
    else
    {
        asm_node(tree, node->left, cur_table, code_error);
        asm_node(tree, node->right, cur_table, code_error);

        if (node->parent->data.types_op == OP_NO)
        {
            asm_opp_compare(tree, node, mode, n_label, code_error);
        }
    }
}

#define PRINT_LABEL(str)                                  \
    if (mode == WHILE)                                    \
    {                                                     \
        PRINT_ASM_PARAM("\t\t" str " while_%d\n", label); \
    }                                                     \
    else                                                  \
    {                                                     \
        PRINT_ASM_PARAM("\t\t" str " if_%d\n", label);    \
    }


void asm_compare (Tree *tree, Node *node, op_command mode, int label, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);
    my_assert(node != NULL, ERR_PTR);

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL("jae");
            break;
        }
        case (BE):
        {
            PRINT_LABEL("jbe");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL("ja");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL("jb");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL("je");
            break;
        }
        case (NE):
        {
            PRINT_LABEL("jne");
            break;
        }
        default:
        {
            break;
        }
    }
}

void asm_opp_compare (Tree *tree, Node *node, op_command mode, int label, int *code_error)
{
    my_assert(node != NULL,    ERR_PTR);
    my_assert(tree != NULL,    ERR_PTR);

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL("jb");
            break;
        }
        case (BE):
        {
            PRINT_LABEL("ja");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL("jbe");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL("jae");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL("jne");
            break;
        }
        case (NE):
        {
            PRINT_LABEL("je");
            break;
        }
        default:
        {
            break;
        }
    }
}

#undef PRINT_LABEL

void asm_print (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);
    my_assert(tree != NULL, ERR_PTR);

    Node *temp_node = node->left;

    while (temp_node != NULL)
    {
        asm_node(tree, temp_node, cur_table, code_error);
        PRINT_ASM("\t\tout\n");

        temp_node = temp_node->left;
    }
}

void asm_input (Tree *tree, Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);
    my_assert(tree != NULL, ERR_PTR);

    Node *temp_node = node->left;

    while (temp_node != NULL)
    {
        PRINT_ASM("\t\tin\n");
        PRINT_ASM_PARAM("\t\tpop [rbx + %d]\n", temp_node->data.ident);

        temp_node = temp_node->left;
    }
}

void asm_return (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(node      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    asm_node(tree, node->left, cur_table, code_error);

    if (node->left->type != CALL_FUNC)
    {
        PRINT_ASM("\t\tpop rax\n");
    }

    PRINT_ASM("\t\tret\n");

    is_ret = true;
}

#undef PRINT_ASM
#undef PRINT_ASM_PARAM

#pragma GCC diagnostic pop

#endif
