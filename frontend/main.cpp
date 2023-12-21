#include "input.h"

int main (int argc, char *argv[])
{
    TREE tree = {};

    int code_error = 0;

    create_tree (&tree, argc, argv, &code_error);

    input_expr (&tree, &code_error);

    PRINT_ERROR ();

    TREE_LOG (&tree, &code_error);

    print_tree (tree.root, tree.info.fp_tree, &code_error);

    print_table_name (&tree, &code_error);

    destroy_tree (&tree, &code_error);

    return 0;
}