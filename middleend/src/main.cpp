#include "../inc/middle.h"

int main(int argc, char *argv[])
{
    int code_error = 0;

    Tree tree = {};
    create_tree(&tree, argc, argv, &code_error);

    read_tree(&tree, &code_error);

    tree.root = tree_simplific(tree.root, &code_error);

    print_tree(tree.root, tree.info.fp_tree, &code_error);

    tree_log(&tree, &code_error);

    PRINT_ERROR;

    return 0;
}
