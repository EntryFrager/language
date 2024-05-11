#ifndef DIFF_CPP
#define DIFF_CPP

#include "diff.h"

NODE *n_diff(TREE *tree, size_t n, char *var, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    for (size_t i = 0; i < n; i++)
    {
        tree->root = diff(tree->root, var, code_error);
        ERR_RET(NULL);

        tree->root = tree_simplific(tree->root, code_error);
        ERR_RET(NULL);

        set_parent(tree->root, NULL);
    }

    return tree->root;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

NODE *diff (NODE *node, char *var, int *code_error)
{
    IS_NODE_PTR_NULL(NULL);

    switch (node->type)
    {
        case (NUM):
        {
            return NUM_(0, NULL);
        }
        case (IDENT):
        {
            if (strcmp(node->data.ident, var) == 0)
            {
                return NUM_(1, NULL);
            }
            else
            {
                return NUM_(0, NULL);
            }
        }
        case (OP):
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    return ADD_(DIF_L, DIF_R);
                }
                case (SUB):
                {
                    return SUB_(DIF_L, DIF_R);
                }
                case (MUL):
                {
                    return ADD_(MUL_(DIF_L, COPY_R), MUL_(COPY_L, DIF_R));
                }
                case (DIV):
                {
                    return DIV_(SUB_(MUL_(DIF_L, COPY_R), MUL_(COPY_L, DIF_R)), MUL_(COPY_R, COPY_R));
                }
                case (POW):
                {
                    if (L_TYPE == NUM && R_TYPE == IDENT)
                    {
                        return MUL_(LN_(COPY_L), MUL_(POW_(COPY_L, COPY_R), DIF_R));
                    }

                    return MUL_(MUL_(COPY_R, POW_(COPY_L, SUB_(COPY_R, NUM_(1, NULL)))), DIF_L);
                }
                case (SIN):
                {
                    return MUL_(COS_(COPY_R), DIF_R);
                }
                case (COS):
                {
                    return MUL_(SIN_(COPY_R), MUL_(NUM_(-1, NULL), DIF_R));
                }
                case (SQRT):
                {
                    return MUL_(MUL_(NUM_(0.5, NULL), POW_(COPY_R, NUM_(-0.5, NULL))), DIF_R);
                }
                case (LN):
                {
                    return MUL_(DIV_(NUM_(1, NULL), COPY_R), DIF_R);
                }
                default:
                {
                    break;
                }
            }
        }
        default:
        {
            break;
        }
    }

    return node;
}

#pragma GCC diagnostic pop

#endif // DIFF_CPP
