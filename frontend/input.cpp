#include "input.h"

static void get_table_name (TREE *tree, int *code_error);

int input_expr (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.buf = get_file_to_str (tree->info.fp_expr, &tree->info.size_file, code_error);
    ERR_RET (*code_error);

    tree->info.buf = skip_isspace (tree->info.buf, tree->info.size_file, code_error);
    ERR_RET (*code_error);

    get_token (tree, code_error);
    ERR_RET (*code_error);
    
    tree->root = get_code (tree->token, code_error);
    ERR_RET (*code_error);

    tree->root = set_parent (tree->root, NULL);
    ERR_RET (*code_error);

    get_table_name (tree, code_error);
    ERR_RET (*code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

void get_table_name (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_MEM);

    tree->table_name = (TABLE_NAME *) calloc (tree->n_ident, sizeof (TABLE_NAME));
    my_assert (tree->table_name != NULL, ERR_MEM);

    size_t count_var = 0;

    for (size_t i = 0; i < tree->n_token; i++)
    {
        if (tree->token[i].type == IDENT)
        {
            for (size_t var_pos = 0; var_pos <= count_var; var_pos++)
            {
                if (tree->table_name[var_pos].ident == NULL)
                {
                    tree->table_name[var_pos].ident = tree->token[i].data.ident;
                    count_var++;

                    break;
                }
                else if (strcmp (tree->table_name[var_pos].ident, tree->token[i].data.ident) == 0)
                {                    
                    break;
                }
            }
        }
    }
    
    tree->n_ident = count_var;

    tree->table_name = (TABLE_NAME *) realloc (tree->table_name, tree->n_ident * sizeof (TABLE_NAME));
    my_assert (tree->table_name != NULL, ERR_MEM);
}