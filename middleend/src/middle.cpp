#ifndef MIDDLE_CPP
#define MIDDLE_CPP

#include "../inc/middle.h"

static Node *add_simplific (Node *node, int *code_error);
static Node *sub_simplific (Node *node, int *code_error);
static Node *mul_simplific (Node *node, int *code_error);
static Node *div_simplific (Node *node, int *code_error);
static Node *pow_simplific (Node *node, int *code_error);

static Node *hanging_tree  (Node *node, Node *hanging_node, Node *parent, int *code_error);
static element eval_node   (int op, element first_value, element second_value, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

Node *tree_simplific (Node *node, int *code_error)
{
    IS_NODE_PTR_NULL(NULL);

    if (node->type == NUM || node->type == IDENT || node->type == CALL_FUNC)
    {
        return node;
    }

    node->left  = tree_simplific(node->left, code_error);
    node->right = tree_simplific(node->right, code_error);

    if (node->type == OP && node->left != NULL && node->right != NULL)
    {
        if (L_TYPE == NUM && R_TYPE == NUM)
        {
            element value = eval_node(node->data.types_op, L_VALUE, R_VALUE, code_error);
            node = NUM_(value, node->parent);
        }
        else
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    node = add_simplific(node, code_error);
                    ERR_RET(NULL);

                    break;
                }
                case (SUB):
                {
                    node = sub_simplific(node, code_error);
                    ERR_RET(NULL);

                    break;
                }
                case (MUL):
                {
                    node = mul_simplific(node, code_error);
                    ERR_RET(NULL);

                    break;
                }
                case (DIV):
                {
                    node = div_simplific(node, code_error);
                    ERR_RET(NULL);

                    break;
                }
                case (POW):
                {
                    node = pow_simplific(node, code_error);
                    ERR_RET(NULL);

                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    ERR_RET (NULL);

    return node;
}

Node *add_simplific (Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);

    IF_NUM(L_TYPE, {if (is_zero(L_VALUE)) {R_RE_HANGING;}})
    IF_NUM(R_TYPE, {if (is_zero(R_VALUE)) {L_RE_HANGING;}})

    return node;
}

Node *sub_simplific (Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);

    IF_NUM(R_TYPE, {if (is_zero(R_VALUE)) {L_RE_HANGING;}})

    return node;
}

Node *mul_simplific (Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);

    IF_NUM(L_TYPE,
           if (is_zero(L_VALUE))
           {
               L_RE_HANGING;
           }
           else if (is_one(L_VALUE))
           {
               R_RE_HANGING;
           })
    IF_NUM(R_TYPE,
          if (is_zero(R_VALUE))
          {
              R_RE_HANGING;
          }
          else if (is_one(R_VALUE))
          {
              L_RE_HANGING;
          })

    return node;
}

Node *div_simplific (Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);

    IF_NUM(L_TYPE,
           if (is_zero(L_VALUE))
           {
               L_RE_HANGING;
           })

    IF_NUM(R_TYPE,
          if (is_one(R_VALUE))
          {
              R_RE_HANGING;
          })

    return node;
}

Node *pow_simplific (Node *node, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);

    IF_NUM(L_TYPE,
           if (is_zero(L_VALUE) || is_one(L_VALUE))
           {
               L_RE_HANGING;
           })
    IF_NUM(R_TYPE,
           if (is_zero(R_VALUE))
           {
               R_VALUE = 1;
               R_RE_HANGING;
           }
           else if (is_one(R_VALUE))
           {
               L_RE_HANGING;
           })

    return node;
}

Node *hanging_tree (Node *node, Node *hanging_node, Node *parent, int *code_error)
{
    my_assert(node != NULL, ERR_PTR);
    my_assert(hanging_node != NULL, ERR_PTR);

    if (parent != NULL)
    {
        if (parent->left == node)
        {
            parent->left = hanging_node;
        }
        else if (parent->right == node)
        {
            parent->right = hanging_node;
        }
    }

    hanging_node->parent = parent;

    if (hanging_node == node->left)
    {
        delete_node(node->right);
    }
    else if (hanging_node == node->right)
    {
        delete_node(node->left);
    }

    free(node);

    ERR_RET(NULL);

    return hanging_node;
}

#define DEF_EVAL(op) first_value op second_value

element eval_node (int op, element first_value, element second_value, int *code_error)
{
    switch (op)
    {
        case (ADD):
        {
            return DEF_EVAL(+);
        }
        case (SUB):
        {
            return DEF_EVAL(-);
        }
        case (MUL):
        {
            return DEF_EVAL(*);
        }
        case (DIV):
        {
            if (!is_zero(second_value))
            {
                return DEF_EVAL(/);
            }
            else
            {
                *code_error |= ERR_DIV_ZERO;
            }
        }
        case (POW):
        {
            return pow(first_value, second_value);
        }
        case (SIN):
        {
            return sin(second_value);
        }
        case (COS):
        {
            return cos(second_value);
        }
        case (SQRT):
        {
            if (second_value >= 0)
            {
                return pow(second_value, 0.5);
            }
            else
            {
                *code_error |= ERR_SQRT_NEGAT;
            }
        }
        case (LN):
        {
            if (second_value > 0)
            {
                return log(second_value);
            }
            else
            {
                *code_error |= ERR_LOG_NEGAT;
            }
        }
        default:
        {
            break;
        }
    }

    return ERR_NO;
}

#undef DEF_EVAl

#pragma GCC diagnostic pop

#endif // MIDDLE_CPP
