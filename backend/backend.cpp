#include "backend.h"

static void print_node         (NODE *node, TREE *tree,  int *n_label,     int *code_error);
static void print_assig        (TREE *tree, NODE *node,  int *code_error);
static void print_expr_asm     (TREE *tree, NODE *node,  int *code_error);
static void print_if           (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static void print_while        (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static void print_condition    (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static void print_compare      (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static void print_opp_compare  (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static void print_print        (TREE *tree, NODE *node,  int *code_error);
static void print_input        (TREE *tree, NODE *node,  int *code_error);
static void print_break        (TREE *tree, NODE *node,  int *n_label,     int *code_error);
static size_t get_number_ident (TREE *tree, char *ident, int *code_error);

void print_asm (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    int n_label = 0;

    print_node (tree->root, tree, &n_label, code_error);

    fprintf (tree->info.fp_tree, "hlt");
}

#define PRINT_ASM(str) fprintf (tree->info.fp_tree, str "\n");
#define PRINT_ASM_PARAM(str, ...) fprintf (tree->info.fp_tree, str "\n", __VA_ARGS__);

void print_node (NODE *node, TREE *tree, int *n_label, int *code_error)
{
    my_assert (tree    != NULL, ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    IS_NODE_PTR_NULL();

    switch (node->type)
    {
        case (OP):
        {
            switch (node->data.types_op)
            {
                case (END_EXPR):
                {
                    print_node (node->left, tree, n_label, code_error);
                    print_node (node->right, tree, n_label, code_error);
                    return;
                }
                case (ASSIG):
                {
                    print_assig (tree, node, code_error);
                    return;
                }
                case (IF):
                {
                    print_if (tree, node, n_label, code_error);
                    return;
                }
                case (WHILE):
                {
                    print_while (tree, node, n_label, code_error);
                    return;
                }
                case (PRINT):
                {
                    print_print (tree, node, code_error);
                    return;
                }
                case (MY_INPUT):
                {
                    print_input (tree, node, code_error);
                    return;
                }
                case (BREAK):
                {
                    print_break (tree, node, n_label, code_error);
                    return;
                }
                default:
                {
                    break;
                }
            }

            break;
        }
        case (NUM):
        case (IDENT):
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }
}

void print_assig (TREE *tree, NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (tree != NULL, ERR_PTR);

    size_t item_ident = get_number_ident (tree, node->left->data.ident, code_error);
    ERR_RET ();

    print_expr_asm (tree, node->right, code_error);

    PRINT_ASM_PARAM ("pop [%d]", item_ident);
}

#define EXPR_ASM(str)                                   \
    print_expr_asm (tree, node->left,  code_error);     \
    print_expr_asm (tree, node->right, code_error);     \
    PRINT_ASM (str);

#define EXPR_ASM_SIDE(str)                              \
    print_expr_asm (tree, node->right, code_error);     \
    PRINT_ASM (str);

void print_expr_asm (TREE *tree, NODE *node, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    IS_NODE_PTR_NULL ();

    switch (node->type)
    {
        case (OP):
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    EXPR_ASM ("add");
                    break;
                }
                case (SUB):
                {
                    EXPR_ASM ("sub");
                    break;
                }
                case (MUL):
                {
                    EXPR_ASM ("mul");
                    break;
                }
                case (DIV):
                {
                    EXPR_ASM ("div");
                    break;
                }
                case (POW):
                {
                    EXPR_ASM ("pow");
                    break;
                }
                case (SQRT):
                {
                    EXPR_ASM_SIDE ("sqrt");
                    break;
                }
                case (LN):
                {
                    EXPR_ASM_SIDE ("ln");
                    break;
                }
                case (SIN):
                {
                    EXPR_ASM_SIDE ("sin");
                    break;
                }
                case (COS):
                {
                    EXPR_ASM_SIDE ("cos");
                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }
        case (NUM):
        {
            PRINT_ASM_PARAM ("push %lg", node->data.value);
            break;
        }
        case (IDENT):
        {
            size_t item_ident = get_number_ident (tree, node->data.ident, code_error);
            ERR_RET ();

            PRINT_ASM_PARAM ("push [%d]", item_ident);

            break;
        }
        default:
        {
            break;
        }
    }
}

#define PRINT_LABEL(str) PRINT_ASM_PARAM (str " label_%d", *n_label);

void print_if (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    print_condition (tree, node->left->left, n_label, code_error);

    int if_label = (*n_label)++;

    print_node (node->left->right, tree, n_label, code_error);

    PRINT_ASM_PARAM ("label_%d:", if_label);
    (*n_label)++;

    print_node (node->right, tree, n_label, code_error);

}

void print_while (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    PRINT_ASM_PARAM ("label_%d:", *n_label);
    int label_while = (*n_label)++;

    print_condition (tree, node->left, n_label, code_error);

    print_node (node->right, tree, n_label, code_error);

    PRINT_ASM_PARAM ("jmp label_%d", label_while);

    PRINT_ASM_PARAM ("label_%d:", *n_label);
    (*n_label)++;
}

void print_condition (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    int cur_lab = *n_label;

    if (node->type == OP && (node->data.types_op == AND || node->data.types_op == OR))
    {
        print_condition (tree, node->left, n_label, code_error);

        if (node->data.types_op == AND)
        {
            print_opp_compare (tree, node, n_label, code_error);
        }
        else
        {
            print_compare (tree, node, n_label, code_error);

            (*n_label)++;
        }

        print_condition (tree, node->right, n_label, code_error);

        print_opp_compare (tree, node, n_label, code_error);

        if (node->data.types_op == OR)
        {
            PRINT_ASM_PARAM ("label_%d:", cur_lab);
        }
    }
    else
    {
        print_expr_asm (tree, node->left, code_error);
        print_expr_asm (tree, node->right, code_error);

        print_opp_compare (tree, node, n_label, code_error);
    }
}

void print_compare (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL ("jae");
            break;
        }
        case (BE):
        {
            PRINT_LABEL ("jbe");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL ("ja");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL ("jb");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL ("je");
            break;
        }
        case (NE):
        {
            PRINT_LABEL ("jne");
            break;
        }
    }
}

void print_opp_compare (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    switch (node->data.types_op)
    {
        case (AE):
        {
            PRINT_LABEL ("ja");
            break;
        }
        case (BE):
        {
            PRINT_LABEL ("jb");
            break;
        }
        case (ABOVE):
        {
            PRINT_LABEL ("jae");
            break;
        }
        case (LESS):
        {
            PRINT_LABEL ("jbe");
            break;
        }
        case (EQUAL):
        {
            PRINT_LABEL ("jne");
            break;
        }
        case (NE):
        {
            PRINT_LABEL ("je");
            break;
        }
    }
}

void print_print (TREE *tree, NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (tree != NULL, ERR_PTR);

    size_t num_ident = get_number_ident (tree, node->right->data.ident, code_error);
    ERR_RET ();

    PRINT_ASM_PARAM ("push [%d]", num_ident);
    PRINT_ASM       ("out");
}

void print_input (TREE *tree, NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (tree != NULL, ERR_PTR);

    size_t num_ident = get_number_ident (tree, node->right->data.ident, code_error);
    ERR_RET ();

    PRINT_ASM       ("in");
    PRINT_ASM_PARAM ("pop [%d]", num_ident);   
}

void print_break (TREE *tree, NODE *node, int *n_label, int *code_error)
{
    my_assert (node != NULL,    ERR_PTR);
    my_assert (tree != NULL,    ERR_PTR);
    my_assert (n_label != NULL, ERR_PTR);

    PRINT_LABEL ("jmp");
}

size_t get_number_ident (TREE *tree, char *ident, int *code_error)
{
    my_assert (tree  != NULL, ERR_PTR);
    my_assert (ident != NULL, ERR_PTR);

    for (size_t i = 0; i < tree->n_ident; i++)
    {
        if (strncmp (ident, tree->table_name[i].ident, str_len (tree->table_name[i].ident)) == 0)
        {
            return i;
        }
    }

    return ERR_NO;
}

#undef PRINT_ASM
#undef PRINT_ASM_PARAM
#undef EXPR_ASM
#undef EXPR_ASM_SIDE
#undef PRINT_LABEL