#ifndef DIFF_H
#define DIFF_H

#include <math.h>

#include "input.h"
#include "tree.h"

NODE *n_diff (TREE *tree, size_t n, char *var, int *code_error);

NODE *diff (NODE *node, char *var, int *code_error);

#endif //DIFF_H
