#ifndef INPUT_CPP
#define INPUT_CPP

#include "../inc/input.h"

void input_expr (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    tree->info.buf = get_file_to_str(tree->info.fp_expr, &tree->info.size_file, code_error);
    ERR_RET();

    tree->info.buf = skip_isspace(tree->info.buf, tree->info.size_file, code_error);
    ERR_RET();

    get_token(tree, code_error);
    ERR_RET();

    init_table_name(&tree->table_name, tree->idents.n_funcs + 1, code_error);

    tree->root = get_code(tree, code_error);
    ERR_RET();

    tree->root = set_parent(tree->root, NULL);
    ERR_RET();
}

#endif // INPUT_CPP
