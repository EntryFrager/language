#ifndef READ_TREE_CPP
#define READ_TREE_CPP

#include "./read_tree.h"

static Node *split_node      (Tree *tree, Node *parent, int *code_error);
static Node *read_node       (Tree *tree, Node *parent, int *code_error);
static Node *read_num        (Tree *tree, Node *parent, int *code_error);
static Node *read_ident      (Tree *tree, Node *parent, int *code_error);
static Node *read_ident_func (Tree *tree, Node *parent, int *code_error);
static Node *read_call_func  (Tree *tree, Node *parent, int *code_error);
static Node *read_op         (Tree *tree, Node *parent, int *code_error);
static Node *read_param      (Tree *tree, Node *parent, int *code_error);
static void read_table_name  (Tree *tree, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

void read_tree (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    tree->info.buf = get_file_to_str(tree->info.fp_tree, &tree->info.size_file, code_error);
    ERR_RET();

    tree->root = split_node(tree, NULL, code_error);
    ERR_RET();

    tree->root = set_parent(tree->root, NULL);
    ERR_RET();

    read_table_name(tree, code_error);
    ERR_RET();

    assert_tree(tree);
}

Node *split_node (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    while (isspace(*tree->info.buf) != 0 || *tree->info.buf == '(' || *tree->info.buf == ')')
    {
        tree->info.buf++;
    }

    if (strncmp(tree->info.buf, "_", 1) == 0)
    {
        tree->info.buf++;
        return NULL;
    }

    Node *node = read_node(tree, parent, code_error);
    ERR_RET(NULL);

    node->left = split_node(tree, node, code_error);
    ERR_RET(NULL);

    node->right = split_node(tree, node, code_error);
    ERR_RET(NULL);

    return node;
}

Node *read_node (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    types type_node = DEF_TYPE;
    int   n_read    = 0;

    sscanf(tree->info.buf, "%d%n", &type_node, &n_read);
    tree->info.buf += n_read;

    while (isdigit(*tree->info.buf) == 0)
    {
        tree->info.buf++;
    }

    switch (type_node)
    {
        case (NUM):
        {
            return read_num(tree, parent, code_error);
        }
        case (IDENT):
        {
            return read_ident(tree, parent, code_error);
        }
        case (IDENT_FUNC):
        {
            return read_ident_func(tree, parent, code_error);
        }
        case (CALL_FUNC):
        {
            return read_call_func(tree, parent, code_error);
        }
        case (OP):
        {
            return read_op(tree, parent, code_error);
        }
        case (PARAM):
        {
            return read_param(tree, parent, code_error);
        }
        default:
        {
            break;
        }
    }

    return NULL;
}

Node *read_num (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    double value = 0;
    int n_read   = 0;

    sscanf(tree->info.buf, "%lf%n", &value, &n_read);
    tree->info.buf += n_read;

    return NUM_(value, parent);
}

Node *read_ident (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    int ident  = 0;
    int n_read = 0;

    sscanf(tree->info.buf, "%d%n", &ident, &n_read);
    tree->info.buf += n_read;

    return IDENT_(ident, parent);
}

Node *read_ident_func (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    int ident_func = 0;
    int n_read     = 0;

    sscanf(tree->info.buf, "%d%n", &ident_func, &n_read);
    tree->info.buf += n_read;

    return IDENT_FUNC_(ident_func, parent);
}

Node *read_call_func (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    int call_func = 0;
    int n_read    = 0;

    sscanf(tree->info.buf, "%d%n", &call_func, &n_read);
    tree->info.buf += n_read;

    return CALL_FUNC_(call_func, parent);
}

Node *read_op (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    op_command op = OP_NO;
    int n_read    = 0;

    sscanf(tree->info.buf, "%d%n", &op, &n_read);
    tree->info.buf += n_read;

    return OP_(op, parent);
}

Node *read_param (Tree *tree, Node *parent, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    int ident  = 0;
    int n_read = 0;

    sscanf(tree->info.buf, "%d%n", &ident, &n_read);
    tree->info.buf += n_read;

    return PARAM_(ident, parent);
}

void read_table_name (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    fopen_init_(tree->info.fp_table_name, tree->info.fp_name_table_name, "a+");

    size_t size_file = 0;
    char *table_name_str = get_file_to_str(tree->info.fp_table_name, &size_file, code_error);
    ERR_RET();

    int n_read = 0;

    sscanf(table_name_str, "%ld%n", &tree->idents.n_ident, &n_read);
    table_name_str += n_read + 2;

    calloc_init_(tree->idents.ident, Ident *, tree->idents.n_ident, sizeof(Ident));

    for (int i = 0; i < tree->idents.n_ident; i++)
    {
        tree->idents.ident[i].name_var = table_name_str;
        tree->idents.ident[i].n_var = i;

        while (isalpha(*table_name_str) || *table_name_str == '_' || isdigit(*table_name_str))
        {
            table_name_str++;
        }

        *table_name_str = '\0';

        while (!isalpha(*table_name_str) && *table_name_str != '_' && !isdigit(*table_name_str))
        {
            table_name_str++;
        }
    }

    sscanf(table_name_str, "%ld%n", &tree->table_name.n_scope, &n_read);
    table_name_str += n_read;

    calloc_init_(tree->table_name.scope_table_name, ScopeTableName **, tree->table_name.n_scope, sizeof(ScopeTableName *));

    int code_table_name = 0;
    size_t n_elem       = 0;

    for (size_t i = 0; i < tree->table_name.n_scope; i++)
    {
        sscanf(table_name_str, "%ld %d%n", &n_elem, &code_table_name, &n_read);
        table_name_str += n_read;

        add_table_name(&tree->table_name, code_table_name, code_error);
        calloc_init_(tree->table_name.scope_table_name[i]->name, Name *, n_elem, sizeof(Name));

        for (size_t j = 0; j < n_elem; j++)
        {
            sscanf(table_name_str, "%d %d%n", &tree->table_name.scope_table_name[i]->name[j].n_var,
                                              &tree->table_name.scope_table_name[i]->name[j].type,
                                              &n_read);
            table_name_str += n_read;
        }
    }

    tree->idents.n_funcs = tree->table_name.n_scope - 1;

    fclose_(tree->info.fp_table_name);
}

#pragma GCC diagnostic pop

#endif // READ_TREE_CPP
