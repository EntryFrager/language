#include "read_tree.h"

static NODE *split_node     (TREE *tree, NODE *parent, int *code_error);
static NODE *read_node      (TREE *tree, NODE *parent, int *code_error);
static NODE *read_num       (TREE *tree, NODE *parent, int *code_error);
static NODE *read_ident     (TREE *tree, NODE *parent, int *code_error);
static NODE *read_op        (TREE *tree, NODE *parent, int *code_error);
static void read_table_name (TREE *tree, int *code_error);

int read_tree (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.buf = get_str_file (tree->info.fp_expr, &tree->info.size_file, code_error);
    ERR_RET (*code_error);

    tree->root = split_node (tree, NULL, code_error);
    ERR_RET (*code_error);

    tree->root = set_parent (tree->root, NULL);
    ERR_RET (*code_error);

    read_table_name (tree, code_error);
    ERR_RET (*code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *split_node (TREE *tree, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    while (isspace (*tree->info.buf))
    {
        tree->info.buf++;
    }

    if (*tree->info.buf == ')')
    {
        tree->info.buf++;
        return NULL;
    }
    else if (strncmp (tree->info.buf, "nil", 3) == 0)
    {
        tree->info.buf += 3;
        return NULL;
    }

    while ((isdigit (*tree->info.buf) == 0 && isalpha (*tree->info.buf) == 0))
    {
        tree->info.buf++;
    }

    NODE *node = read_node (tree, parent, code_error);
    ERR_RET (NULL);

    node->left = split_node (tree, node, code_error);
    ERR_RET (NULL);

    while (*tree->info.buf == ')' || isspace (*tree->info.buf) != 0)
    {
        tree->info.buf++;
    }

    node->right = split_node (tree, node, code_error);
    ERR_RET (NULL);

    return node;
}

NODE *read_node (TREE *tree, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    types type_node = DEF_TYPE;
    size_t n_read = 0;

    sscanf (tree->info.buf, "%d%n", &type_node, &n_read);
    tree->info.buf += n_read;

    while (isspace (*tree->info.buf))
    {
        tree->info.buf++;
    }

    switch (type_node)
    {
        case (NUM):
        {
            return read_num (tree, parent, code_error);
        }
        case (IDENT):
        {
            return read_ident (tree, parent, code_error);
        }
        case (OP):
        {
            return read_op (tree, parent, code_error);
        }
        default:
        {
            break;
        }
    }

    return NULL;
}

NODE *read_num (TREE *tree, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    double value = 0;
    size_t n_read= 0;

    sscanf (tree->info.buf, "%lf%n", &value, &n_read);
    tree->info.buf += n_read;

    return NUM_ (value, parent);
}

NODE *read_ident (TREE *tree, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    char *ident = read_ident (&tree->info.buf, code_error);
    ERR_RET (NULL);

    return IDENT_ (ident, parent);
}

NODE *read_op (TREE *tree, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    op_comand op = OP_NO;
    size_t n_read = 0;

    sscanf (tree->info.buf, "%d%n", &op, &n_read);
    tree->info.buf += n_read;

    return OP_ (op, parent);
}

void read_table_name (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.fp_table_name = fopen (tree->info.fp_name_table_name, "a+");
    my_assert (tree->info.fp_table_name != NULL, ERR_FOPEN);

    size_t size_file = 0;
    char *table_name = get_str_file (tree->info.fp_table_name, &size_file, code_error);
    ERR_RET ();

    tree->n_ident = get_n_lines (table_name, code_error);
    ERR_RET ();

    tree->table_name = (TABLE_NAME *) calloc (tree->n_ident, sizeof (TABLE_NAME));
    my_assert (tree->table_name != NULL, ERR_MEM);

    for (size_t i = 0; i < tree->n_ident; i++)
    {
        tree->table_name[i].ident = table_name;

        while (isalpha (*table_name))
        {
            table_name++;
        }

        *table_name = '\0';

        while (!isalpha (*table_name))
        {
            table_name++;
        }
    }

    FCLOSE_ (tree->info.fp_table_name);
}