#include "backend.h"

int main (int argc, char *argv[])
{
    TREE tree = {};

    int code_error = 0;

    create_tree (&tree, argc, argv, &code_error);

    read_tree (&tree, &code_error);

    print_asm (&tree, &code_error);

    PRINT_ERROR ();

    return 0;
}