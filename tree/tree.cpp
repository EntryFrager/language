#ifndef TREE_CPP
#define TREE_CPP

#include "tree.h"

static const char *FP_NAME_EXPR       = "../expr.txt";
static const char *FP_NAME_TREE       = "../tree.txt";
static const char *FP_NAME_TABLE_NAME = "../table_name.txt";

static Node *calloc_node (Node *left, Node *right, Node *parent, int *code_error);

int create_tree (Tree *tree, int argc, char *argv[], int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    tree->root = calloc_node(NULL, NULL, NULL, code_error);
    ERR_RET(ERR_NO);

    tree->is_init = true;

    if (argc == 3)
    {
        tree->info.fp_name_expr = argv[1];
        tree->info.fp_name_tree = argv[2];
    }
    else if (argc == 2)
    {
        tree->info.fp_name_expr = argv[1];
        tree->info.fp_name_tree = FP_NAME_TREE;
    }
    else
    {
        tree->info.fp_name_expr = FP_NAME_EXPR;
        tree->info.fp_name_tree = FP_NAME_TREE;
    }

    tree->info.fp_name_table_name = FP_NAME_TABLE_NAME;

    fopen_init_(tree->info.fp_expr, tree->info.fp_name_expr, "r + b");
    fopen_init_(tree->info.fp_tree, tree->info.fp_name_tree, "w + b");

    return ERR_NO;
}

Node *calloc_node (Node *left, Node *right, Node *parent, int *code_error)
{
    calloc_(node, Node *, 1, sizeof(Node))

    node->left   = left;
    node->right  = right;
    node->parent = parent;

    return node;
}

Node *create_node_num (element value, Node *left, Node *right, Node *parent, int *code_error)
{
    Node *node = calloc_node(left, right, parent, code_error);
    ERR_RET(NULL);

    node->data.value = value;
    node->type       = NUM;

    return node;
}

Node *create_node_ident (int ident, Node *left, Node *right, Node *parent, int *code_error)
{
    Node *node = calloc_node(left, right, parent, code_error);
    ERR_RET(NULL);

    node->data.ident = ident;
    node->type       = IDENT;

    return node;
}

Node *create_node_op (op_command types_op, Node *left, Node *right, Node *parent, int *code_error)
{
    Node *node = calloc_node(left, right, parent, code_error);
    ERR_RET(NULL);

    node->data.types_op = types_op;
    node->type          = OP;

    return node;
}

Node *create_node (types type, Node *left, Node *right, Node *parent, int *code_error)
{
    Node *node = calloc_node(left, right, parent, code_error);
    ERR_RET(NULL);

    node->type = type;

    return node;
}

Node *set_parent (Node *node, Node *parent)
{
    IS_NODE_PTR_NULL(NULL);

    node->parent = parent;

    set_parent(node->left, node);
    set_parent(node->right, node);

    return node;
}

int delete_node (Node *node)
{
    IS_NODE_PTR_NULL(ERR_NO);

    delete_node(node->left);

    delete_node(node->right);

    free(node);

    return ERR_NO;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

Node *copy_tree (Node *node, Node *parent, int *code_error)
{
    IS_NODE_PTR_NULL(NULL);

    Node *copy_node = NULL;

    switch (node->type)
    {
        case (NUM):
        {
            copy_node = NUM_(node->data.value, parent);
            ERR_RET (NULL);

            break;
        }
        case (OP):
        {
            copy_node = OP_(node->data.types_op, parent);
            ERR_RET (NULL);

            break;
        }
        case (IDENT):
        {
            copy_node = IDENT_(node->data.ident, parent);
            ERR_RET (NULL);

            break;
        }
        default:
        {
            break;
        }
    }

    copy_node->left = copy_tree(node->left, copy_node, code_error);
    ERR_RET(NULL);

    copy_node->right = copy_tree(node->right, copy_node, code_error);
    ERR_RET(NULL);

    return copy_node;
}

void print_tree (Node *node, FILE *stream, int *code_error)
{
    my_assert(stream != NULL, ERR_PTR);

    if(!node)
    {
        fprintf(stream, "_ ");
        return;
    }

    fprintf(stream, "( ");

    switch (node->type)
    {
        case (OP):
        {
            fprintf(stream, "%d %d ", node->type, node->data.types_op);
            break;
        }
        case (NUM):
        {
            fprintf(stream, "%d %lg ", node->type, node->data.value);
            break;
        }
        case (IDENT):
        {
            fprintf(stream, "%d %d ", node->type, node->data.ident);
            break;
        }
        case (IDENT_FUNC):
        {
            fprintf(stream, "%d %d ", node->type, node->data.ident);
            break;
        }
        case (CALL_FUNC):
        {
            fprintf(stream, "%d %d ", node->type, node->data.ident);
            break;
        }
        default:
        {
            break;
        }
    }

    print_tree(node->left, stream, code_error);
    ERR_RET();

    print_tree(node->right, stream, code_error);
    ERR_RET();

    fprintf(stream, ") ");
}

#pragma GCC diagnostic pop

#define print(str, ...) fprintf(tree->info.fp_table_name, str, __VA_ARGS__);

void print_table_name (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    size_t n_ident = tree->table_name.n_ident;

    fopen_init_(tree->info.fp_table_name, tree->info.fp_name_table_name, "w+");

    print("%ld\n\n", n_ident);

    for (size_t i = 0; i < n_ident; i++)
    {
        print("%d %s\n", tree->table_name.pairs_num_ident[i].number, tree->table_name.pairs_num_ident[i].ident);
    }

    print("\n%ld\n\n%ld -1\n", tree->table_name.n_ident_func + 1, tree->table_name.n_ident_func);

    for (size_t i = 0; i < n_ident; i++)
    {
        if (tree->table_name.pairs_num_ident[i].type == IDENT_FUNC)
        {
            print("%d %d\n", tree->table_name.pairs_num_ident[i].number, tree->table_name.pairs_num_ident[i].type);
        }
    }

    int id_scope = 0;

    for (size_t i = 0; i < n_ident; i++)
    {
        if (tree->table_name.pairs_num_ident[i].type == IDENT_FUNC)
        {
            print("\n%d\n", id_scope);

            id_scope++;
        }
        else
        {
            print("%d %d\n", tree->table_name.pairs_num_ident[i].number, tree->table_name.pairs_num_ident[i].type);
        }
    }

    fclose_(tree->info.fp_table_name);
}

int destroy_tree (Tree *tree, int *code_error)
{
    assert_tree(tree);

    delete_node(tree->root);
    tree->root = NULL;

    tree->is_init = false;

    tree->info.fp_name_expr = NULL;
    tree->info.fp_name_tree = NULL;
    tree->info.fp_name_table_name = NULL;

    fclose_(tree->info.fp_tree);
    fclose_(tree->info.fp_expr);

    tree->info.size_file = 0;

    free(tree->token);
    free(tree->info.buf);
    free(tree->table_name.pairs_num_ident);

    tree->token                      = NULL;
    tree->info.buf                   = NULL;
    tree->table_name.pairs_num_ident = NULL;
    tree->table_name.n_ident         = 0;
    tree->table_name.n_ident_func    = 0;
    tree->n_token                    = 0;

    return ERR_NO;
}

#ifdef DEBUG_TREE

void tree_verificator (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, TREE_ERR_PTR);

    my_assert(tree->is_init == true, TREE_NOT_INIT);

    if (*code_error != ERR_NO)
    {
        my_assert(tree->root != NULL, TREE_ERR_ROOT_PTR);

        if (*code_error != ERR_NO)
        {
            node_verificator(tree->root, code_error);
        }
    }
}

void node_verificator (Node *node, int *code_error)
{
    IS_NODE_PTR_NULL();

    if (node->parent != NULL)
    {
        my_assert(node->parent->left == node || node->parent->right == node, NODE_ERR);
    }

    node_verificator(node->left, code_error);
    node_verificator(node->right, code_error);
}

#endif

#endif // TREE_CPP
