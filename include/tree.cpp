#include "tree.h"

static NODE *calloc_node (NODE *left, NODE *right, NODE *parent, int *code_error);

int create_tree (TREE *tree, int argc, char *argv[], int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->root = calloc_node (NULL, NULL, NULL, code_error);
    ERR_RET (ERR_NO);

    tree->is_init = true;

    if (argc == 3)
    {
        tree->info.fp_name_expr = argv[1];
        tree->info.fp_name_tree = argv[2];

        tree->info.fp_expr = fopen (tree->info.fp_name_expr, "r + b");
        my_assert (tree->info.fp_expr != NULL, ERR_FOPEN);

        tree->info.fp_tree = fopen (tree->info.fp_name_tree, "w + b");
        my_assert (tree->info.fp_tree != NULL, ERR_FOPEN);
    }
    else
    {
        tree->info.fp_name_expr = "../expr.txt";
        tree->info.fp_name_tree = "../tree.txt";
    }

    tree->info.fp_name_table_name = "../table_name.txt";

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = "../DEBUG/file_err_tree.txt";
    tree->info.fp_dot_name       = "../DEBUG/dump.dot";
    tree->info.fp_name_html      = "../DEBUG/dot.html";
    tree->info.fp_image          = "../DEBUG/dot.svg";
#endif

    return ERR_NO;
}

NODE *calloc_node (NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = (NODE *) calloc (1, sizeof (NODE));
    my_assert (node != NULL, ERR_MEM);
    node->left      = left;
    node->right     = right;
    node->parent    = parent;

    return node;
}

NODE *create_node_num (ELEMENT value, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    ERR_RET (NULL);
    
    node->data.value = value;
    node->type       = NUM;

    return node;
}

NODE *create_node_ident (char *ident, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    ERR_RET (NULL);

    node->data.ident = ident;
    node->type       = IDENT;

    return node;
}

NODE *create_node_op (op_comand types_op, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    ERR_RET (NULL);

    node->data.types_op = types_op;
    node->type          = OP;

    return node;
}

NODE *create_node (types type, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    ERR_RET (NULL);

    node->type = type;

    return node;
}

NODE *set_parent (NODE *node, NODE *parent)
{
    IS_NODE_PTR_NULL (NULL);

    node->parent = parent;

    set_parent (node->left, node);
    set_parent (node->right, node);

    return node;
}

int delete_node (NODE *node)
{
    IS_NODE_PTR_NULL (ERR_NO);

    delete_node (node->left);

    delete_node (node->right);

    free (node);

    return ERR_NO;
}

NODE *copy_tree (NODE *node, NODE *parent, int *code_error)
{
    IS_NODE_PTR_NULL (NULL);

    NODE *copy_node = NULL;

    switch (node->type)
    {
        case (NUM):
        {
            copy_node = NUM_ (node->data.value, parent);
            ERR_RET (NULL);

            break;
        }
        case (OP):
        {
            copy_node = OP_ (node->data.types_op, parent);
            ERR_RET (NULL);
            
            break;
        }
        case (IDENT):
        {
            copy_node = IDENT_ (node->data.ident, parent);
            ERR_RET (NULL);

            break;
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }

    copy_node->left  = copy_tree (node->left, copy_node, code_error);
    ERR_RET (NULL);

    copy_node->right = copy_tree (node->right, copy_node, code_error);
    ERR_RET (NULL);

    return copy_node;
}

void print_tree (NODE *node, FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    if (!node)
    {
        fprintf (stream, "nil ");
        return;
    }
    
    fprintf (stream, "( ");

    switch (node->type)
    {
        case (OP):
        {
            fprintf (stream, "%d %d ", node->type, node->data.types_op);
            break;
        }
        case (NUM):
        {
            fprintf (stream, "%d %lg ", node->type, node->data.value);
            break;
        }
        case (IDENT):
        {
            fprintf (stream, "%d %s ", node->type, node->data.ident);
            break;
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }

    print_tree (node->left, stream, code_error);
    ERR_RET ();

    print_tree (node->right, stream, code_error);
    ERR_RET ();

    fprintf (stream, ") ");
}


void print_table_name (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.fp_table_name = fopen (tree->info.fp_name_table_name, "w+");
    my_assert (tree->info.fp_table_name != NULL, ERR_FOPEN);

    for (size_t i = 0; i < tree->n_ident; i++)
    {
        fprintf (tree->info.fp_table_name, "%s\n", tree->table_name[i].ident);
    }

    FCLOSE_ (tree->info.fp_table_name);
}

int destroy_tree (TREE *tree, int *code_error)
{
    assert_tree (tree, ERR_NO);
    
    delete_node (tree->root);
    tree->root = NULL;

    tree->is_init = false;

    tree->info.fp_name_expr = NULL;
    tree->info.fp_name_tree = NULL;
    tree->info.fp_name_table_name = NULL; 

    FCLOSE_ (tree->info.fp_tree);
    FCLOSE_ (tree->info.fp_expr);

    tree->info.size_file = 0;

    free (tree->token);
    free (tree->info.buf);
    free (tree->table_name);

    tree->token            = NULL;
    tree->info.buf         = NULL;
    tree->table_name  = NULL;
    tree->n_token          = 0;
    tree->n_ident = 0;

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = NULL;
    tree->info.fp_dot_name       = NULL;
    tree->info.fp_name_html      = NULL;
    tree->info.fp_image          = NULL;
#endif

    return ERR_NO;
}

#ifdef DEBUG_TREE

#define VERIF_EXPR(expr, name_error) if (!(expr)) {code_error |= name_error;}

int tree_verificator (TREE *tree)
{
    int code_error = 0;

    VERIF_EXPR (tree != NULL, TREE_ERR_PTR)

    VERIF_EXPR (tree->is_init == true, TREE_NOT_INIT)

    VERIF_EXPR (tree->root != NULL, TREE_ERR_ROOT_PTR)

    return code_error;
}

int node_verificator (NODE *node)
{
    int code_error = 0;

    IS_NODE_PTR_NULL (ERR_NO);

    if (node->parent != NULL)
    {
        VERIF_EXPR (node->parent->left == node || node->parent->right == node, NODE_ERR)
    }

    code_error |= node_verificator (node->left);
    code_error |= node_verificator (node->right);

    return code_error;
}

#undef VERIF_EXPR

#endif