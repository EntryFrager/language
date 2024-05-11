#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>

#include "../../tree/tree.h"
#include "../../Include_file/error/error.h"
#include "../../Include_file/utilities/utils.h"

void input_expr (Tree *tree, int *code_error);

void get_token  (Tree *tree, int *code_error);

Node *get_code  (Tree *tree, int *code_error);

#endif //INPUT_H
