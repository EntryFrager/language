#ifndef BACKEND_H
#define BACHEND_H

#include "../../tree/tree.h"
#include "../../tree/read_tree.h"

typedef struct {
    int start_label_body = 0;
    int end_label_body   = 0;
} LabelBody;

void print_asm_code (Tree *tree, int *code_error);

#endif // BACKEND_H
