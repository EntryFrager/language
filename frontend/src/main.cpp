#include "../inc/input.h"

int main (int argc, char *argv[])
{
    int code_error = 0;

    Tree tree = {};
    create_tree(&tree, argc, argv, &code_error);

    input_expr(&tree, &code_error);

    PRINT_ERROR;

    tree_log(&tree, &code_error);

    print_tree(tree.root, tree.info.fp_tree, &code_error);

    print_table_name(&tree, &code_error);

    destroy_tree(&tree, &code_error);

    return 0;
}
