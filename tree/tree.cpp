#ifndef TREE_CPP
#define TREE_CPP

#include "tree.h"

static const char *FP_NAME_EXPR       = "./files/expr.txt";
static const char *FP_NAME_TREE       = "./files/tree.txt";
static const char *FP_NAME_ASM        = "./files/code.asm";
static const char *FP_NAME_TABLE_NAME = "./files/table_name.txt";

static Node           *calloc_node              (Node *left, Node *right, Node *parent, int *code_error);
static ScopeTableName *init_scope_table_name    (int code_table_name, int *code_error);
static void            realloc_scope_table_name (ScopeTableName *scope_table_name, size_t new_size, int *code_error);

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
    tree->info.fp_name_asm        = FP_NAME_ASM;

    fopen_init_(tree->info.fp_expr, tree->info.fp_name_expr, "r + b");
    fopen_init_(tree->info.fp_tree, tree->info.fp_name_tree, "r + b");
    fopen_init_(tree->info.fp_asm,  tree->info.fp_name_asm , "w");

    return ERR_NO;
}

Node *calloc_node (Node *left, Node *right, Node *parent, int *code_error)
{
    calloc_(node, Node *, 1, sizeof(Node));

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

Node *create_node_ident (types type, int ident, Node *left, Node *right, Node *parent, int *code_error)
{
    Node *node = calloc_node(left, right, parent, code_error);
    ERR_RET(NULL);

    node->data.ident = ident;
    node->type       = type;

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
        case (IDENT_FUNC):
        case (CALL_FUNC):
        case (PARAM):
        {
            fprintf(stream, "%d %d ", node->type, node->data.ident);
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

void init_table_name (TableName *table_name, size_t n_scope, int *code_error)
{
    calloc_init_(table_name->scope_table_name, ScopeTableName **, n_scope, n_scope * sizeof(ScopeTableName *));

    table_name->n_scope   = n_scope;
    table_name->cur_scope = 0;

    for (size_t i = 0; i < n_scope; i++)
    {
        table_name->scope_table_name[i] = NULL;
    }
}

ScopeTableName *init_scope_table_name (int code_table_name, int *code_error)
{
    calloc_(scope_table_name, ScopeTableName *, 1, sizeof(ScopeTableName));
    calloc_init_(scope_table_name->name, Name *, 1, sizeof(Name));

    scope_table_name->code_table_name = code_table_name;

    scope_table_name->n_elem       = 1;
    scope_table_name->cur_elem     = 0;
    scope_table_name->n_call_func  = 0;

    return scope_table_name;
}

void add_table_name (TableName *table_name, int code_table_name, int *code_error)
{
    my_assert(table_name != NULL, ERR_PTR);

    table_name->scope_table_name[table_name->cur_scope++] = init_scope_table_name(code_table_name, code_error);
}

void add_table_name_elem (ScopeTableName *scope_table_name, int n_var, types type_var, int *code_error)
{
    my_assert(scope_table_name != NULL, ERR_PTR);

    for (size_t i = 0; i < scope_table_name->cur_elem; i++)
    {
        if (n_var == scope_table_name->name[i].n_var && type_var == scope_table_name->name[i].type)
        {
            return;
        }
    }

    if (scope_table_name->cur_elem == scope_table_name->n_elem)
    {
        realloc_scope_table_name(scope_table_name, scope_table_name->n_elem * 2, code_error);
    }

    scope_table_name->name[scope_table_name->cur_elem].n_var = n_var;
    scope_table_name->name[scope_table_name->cur_elem].type  = type_var;
    scope_table_name->cur_elem++;
}

void realloc_scope_table_name (ScopeTableName *scope_table_name, size_t new_size, int *code_error)
{
    my_assert(scope_table_name != NULL, ERR_PTR);

    scope_table_name->n_elem = new_size;

    realloc_(scope_table_name->name, Name *, new_size * sizeof(Name));
}

#define print_name_table(str, ...) fprintf(tree->info.fp_table_name, str, __VA_ARGS__);

void print_table_name (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    fopen_init_(tree->info.fp_table_name, tree->info.fp_name_table_name, "w+");

    print_name_table("%ld\n\n", tree->idents.n_ident);

    for (size_t i = 0; i < tree->idents.n_ident; i++)
    {
        print_name_table("%s %d %d\n", tree->idents.ident[i].name_var, tree->idents.ident[i].n_scope, tree->idents.ident[i].n_var);
    }

    print_name_table("\n%ld\n", tree->table_name.n_scope);

    for (size_t cur_scope = 0; cur_scope < tree->table_name.n_scope; cur_scope++)
    {
        print_name_table("\n%ld %d\n", tree->table_name.scope_table_name[cur_scope]->n_elem,
                                       tree->table_name.scope_table_name[cur_scope]->code_table_name);

        for (size_t cur_name = 0; cur_name < tree->table_name.scope_table_name[cur_scope]->n_elem; cur_name++)
        {
            print_name_table("%d %d\n", tree->table_name.scope_table_name[cur_scope]->name[cur_name].n_var,
                                        tree->table_name.scope_table_name[cur_scope]->name[cur_name].type);
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

    tree->info.fp_name_asm  = NULL;
    tree->info.fp_name_expr = NULL;
    tree->info.fp_name_tree = NULL;
    tree->info.fp_name_table_name = NULL;

    fclose_(tree->info.fp_tree);
    fclose_(tree->info.fp_expr);
    fclose_(tree->info.fp_asm);

    tree->info.size_file = 0;

    free(tree->token);
    free(tree->info.buf);
    free(tree->idents.ident);

    for (size_t i = 0; i < tree->table_name.n_scope; i++)
    {
        free(tree->table_name.scope_table_name[i]->name);
        free(tree->table_name.scope_table_name[i]);
    }

    free(tree->table_name.scope_table_name);

    tree->token                       = NULL;
    tree->info.buf                    = NULL;
    tree->idents.ident                = NULL;
    tree->table_name.scope_table_name = NULL;

    tree->n_token              = 0;
    tree->idents.n_ident       = 0;
    tree->idents.cur_ident     = 0;
    tree->idents.n_funcs       = 0;
    tree->table_name.cur_scope = 0;
    tree->table_name.n_scope   = 0;

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
