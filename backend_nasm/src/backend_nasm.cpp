#ifndef BACKEND_CPP
#define BACKEND_CPP

#include "../inc/backend_nasm.h"

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
static char *get_func_name  (Identificators *idents, int ident_func, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wswitch-enum"

#define PRINT_ASM(str) fprintf(tree->info.fp_asm, str);
#define PRINT_ASM_PARAM(str, ...) fprintf(tree->info.fp_asm, str, __VA_ARGS__);

void print_asm_code (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    PRINT_ASM("global main\n\n"
              "extern print\n\n"
              "extern input\n\n"
              "section .text\n\n"
              "main:\n"
              "\t\tpush rbp\n"
              "\t\tmov rbp, rsp\n\n"
              "\t\txor rax, rax\n"
              "\t\txor r11, r11\n"
              "\t\txor r12, r12\n"
              "\t\txor r15, r15\n\n");

    asm_funcs(tree, tree->root, code_error);

    PRINT_ASM("section .data\n\n"
              "ram: times 10000 dq 0\n");
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

    char *func_name = get_func_name(&tree->idents, node->data.ident, code_error);

    if (strcmp(func_name, "main") != 0)
    {
        PRINT_ASM_PARAM("\n%s:\n"
                        "\t\tpush rbp\n"
                        "\t\tmov rbp, rsp\n\n", func_name);
    }

    ScopeTableName *func_table_name = NULL;

    for (size_t i = 0; i < tree->table_name.n_scope; i++)
    {
        if (node->data.ident == tree->table_name.scope_table_name[i]->code_table_name)
        {
            func_table_name = tree->table_name.scope_table_name[i];
        }
    }

    asm_body(tree, node->right, func_table_name, code_error);

    if (!is_ret)
    {
        PRINT_ASM("\t\tmov rsp, rbp\n"
                  "\t\tpop rbp\n\n"
                  "\t\tret\n");
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
            PRINT_ASM_PARAM("\t\tpush qword [ram + (r15 + %d) * 8]\n", node->data.ident);
            break;
        }
        case (CALL_FUNC):
        {
            asm_call_func(tree, node, cur_table, code_error);
            break;
        }
        case (PARAM):
        {
            PRINT_ASM_PARAM("\t\tpush qword [ram + (r15 + %d) * 8]\n", node->data.ident);
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

    PRINT_ASM_PARAM("\t\tpush r15\n"
                    "\t\tadd r15, %ld\n", cur_table->n_elem - cur_table->n_call_func);

    PRINT_ASM_PARAM("\t\tcall %s\n\n", get_func_name(&tree->idents, node->data.ident, code_error));

    PRINT_ASM("\t\tpop r15\n"
              "\t\tpush rax\n\n");
}

void asm_params (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    Node *temp_node = node;

    for (int i = 0; temp_node != NULL; i++)
    {
        asm_node(tree, temp_node, cur_table, code_error);

        PRINT_ASM_PARAM("\t\tpop qword [ram + (r15 + %ld) * 8]\n\n", cur_table->n_elem + i - cur_table->n_call_func);

        temp_node = temp_node->left;
    }
}

#define EXPR_ASM(str)                                   \
    asm_node(tree, node->left,  cur_table, code_error); \
    asm_node(tree, node->right, cur_table, code_error); \
    PRINT_ASM("\t\tpop r11\n"                           \
              "\t\tpop r12\n"                           \
              "\t\tmovq xmm0, r11\n"                    \
              "\t\tmovq xmm1, r12\n"                    \
              "\t\t" str " xmm1, xmm0\n"                \
              "\t\tmovq r11, xmm1\n"                    \
              "\t\tpush r11\n");

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
            EXPR_ASM("addsd");
            break;
        }
        case (SUB):
        {
            EXPR_ASM("subsd");
            break;
        }
        case (MUL):
        {
            EXPR_ASM("mulsd");
            break;
        }
        case (DIV):
        {
            EXPR_ASM("divsd");
            break;
        }
        case (SQRT):
        {
            PRINT_ASM("\t\tpop r11\n"
                      "\t\tmovq xmm0, r11\n"
                      "\t\tsqrtsd xmm0, xmm0\n"
                      "\t\tmovq r11, xmm0\n"
                      "\t\tpush r11\n");
            break;
        }
        case (ASSIG):
        {
            int item_ident = node->left->data.ident;
            asm_node(tree, node->right, cur_table, code_error);
            PRINT_ASM_PARAM("\t\tpop qword [ram + (r15 + %d) * 8]\n\n", item_ident);

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
            PRINT_ASM_PARAM("\t\tjmp .while_%d\n\n", while_label - 1);
            break;
        }
        case (CONTINUE):
        {
            PRINT_ASM_PARAM("\t\tjmp .while_%d\n\n", while_label - 2);
            break;
        }
        case (ABORT):
        {
            PRINT_ASM("\t\tmov rax, 60\n"
                      "\t\tsyscall\n\n");
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
            PRINT_ASM_PARAM("\t\tjmp .end_if_%d\n", end_if);
            PRINT_ASM_PARAM("\t.if_%d:\n", end_label_body)
        }

        temp_node = temp_node->right;
    }

    PRINT_ASM_PARAM("\t.end_if_%d:\n", end_if);
}

void asm_while (Tree *tree, Node *node, ScopeTableName *cur_table, int *code_error)
{
    my_assert(node      != NULL, ERR_PTR);
    my_assert(tree      != NULL, ERR_PTR);
    my_assert(cur_table != NULL, ERR_PTR);

    int start_label_body = while_label++;
    int end_label_body   = while_label;

    PRINT_ASM_PARAM("\t.while_%d:\n", start_label_body);

    asm_condition(tree, node->left, cur_table, WHILE, end_label_body, code_error);

    while_label++;

    asm_node(tree, node->right, cur_table, code_error);

    PRINT_ASM_PARAM("\t\tjmp .while_%d\n", end_label_body);
    PRINT_ASM_PARAM("\t.while_%d:\n", end_label_body);
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
            PRINT_ASM_PARAM("\t.if_%d:\n", cur_label);
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

#define PRINT_LABEL(cmp)                                  \
    PRINT_ASM("\t\t" cmp " xmm1, xmm0\n"                  \
              "\t\tmovq rax, xmm1\n"                      \
              "\t\ttest rax, rax\n");                     \
    if (mode == WHILE)                                    \
    {                                                     \
        PRINT_ASM_PARAM("\t\tjnz .while_%d\n\n", label);  \
    }                                                     \
    else                                                  \
    {                                                     \
        PRINT_ASM_PARAM("\t\tjnz .if_%d\n\n", label);     \
    }


void asm_compare (Tree *tree, Node *node, op_command mode, int label, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);
    my_assert(node != NULL, ERR_PTR);

    PRINT_ASM("\t\tpop r11\n"
              "\t\tpop r12\n"
              "\t\tmovq xmm0, r11\n"
              "\t\tmovq xmm1, r12\n");

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL("cmpnltsd");
            break;
        }
        case (BE):
        {
            PRINT_LABEL("cmplesd");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL("cmpnlesd");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL("cmpltsd");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL("cmpeqsd");
            break;
        }
        case (NE):
        {
            PRINT_LABEL("cmpneqsd");
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
    my_assert(node != NULL, ERR_PTR);
    my_assert(tree != NULL, ERR_PTR);

    PRINT_ASM("\t\tpop r11\n"
              "\t\tpop r12\n"
              "\t\tmovq xmm0, r11\n"
              "\t\tmovq xmm1, r12\n");

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL("cmpltsd");
            break;
        }
        case (BE):
        {
            PRINT_LABEL("cmpnlesd");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL("cmplesd");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL("cmpnltsd");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL("cmpneqsd");
            break;
        }
        case (NE):
        {
            PRINT_LABEL("cmpeqsd");
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
        PRINT_ASM("\t\tcall print\n\n");

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
        PRINT_ASM("\t\tcall input\n");
        PRINT_ASM_PARAM("\t\tmov qword [ram + (r15 + %d) * 8], rax\n\n", temp_node->data.ident);

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
        PRINT_ASM("\t\tpop rax\n\n");
    }

    PRINT_ASM("\t\tmov rsp, rbp\n"
              "\t\tpop rbp\n\n"
              "\t\tret\n\n");

    is_ret = true;
}

#undef PRINT_ASM
#undef PRINT_ASM_PARAM

#pragma GCC diagnostic pop

char *get_func_name (Identificators *idents, int ident_func, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);

    for (size_t i = 0; i < idents->n_ident; i++)
    {
        if (idents->ident[i].n_scope == -1 && idents->ident[i].n_var == ident_func)
        {
            return idents->ident[i].name_var;
        }
    }

    return NULL;
}

#endif
