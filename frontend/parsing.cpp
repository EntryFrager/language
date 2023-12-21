#include "input.h"

static NODE *get_op          (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_node_op     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_if_else     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_if          (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_while       (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_stdin       (TOKEN *token, op_comand type, size_t *pos, int *code_error);
static NODE *get_break       (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_condition   (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_compare     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_expr        (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_add_sub     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_mul_div     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_pow         (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_bracket     (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_trig        (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_var         (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_num         (TOKEN *token, size_t *pos, int *code_error);

NODE *get_code (TOKEN *token, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    
    size_t pos = 0;

    NODE *node = get_op (token, &pos, code_error);

    syntax_assert (token[pos].type == DEF_TYPE);

    ERR_RET (NULL);

    return node;
}

NODE *get_op (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == OPEN_BRACE);
    INCREM;

    NODE *node = OP_ (END_EXPR, NULL);

    NODE *temp_node = node;

    while (CUR_TOK.data.types_op != CLOSE_BRACE)
    {
        node->left = get_node_op (token, pos, code_error);
        ERR_RET (NULL);

        if (CUR_TOK.data.types_op == CLOSE_BRACE)
        {
            break;
        }
        
        node->right = OP_ (END_EXPR, NULL);
        node = node->right;
    }

    if (CUR_TOK.type != OP)
    {
        temp_node->right = get_op (token, pos, code_error);
        ERR_RET (NULL);
    }
    else if (CUR_TOK.data.types_op != CLOSE_BRACE)
    {
        temp_node->right = get_op (token, pos, code_error);
        ERR_RET (NULL);
    }

    INCREM;

    return temp_node;
}

NODE *get_node_op (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    NODE *node = NULL;

    switch (CUR_TOK.type)
    {
        case (OP):
        {
            switch (CUR_TOK.data.types_op)
            {
                case (IF):
                {
                    node = get_if_else (token, pos, code_error);
                    break;
                }
                case (WHILE):
                {
                    node = get_while (token, pos, code_error);
                    break;
                }
                case (PRINT):
                {
                    node = get_stdin (token, PRINT, pos, code_error);
                    break;
                }
                case (MY_INPUT):
                {
                    node = get_stdin (token, MY_INPUT, pos, code_error);
                    break;
                }
                case (OPEN_BRACE):
                {
                    node = get_op (token, pos, code_error);
                    break;
                }
                case (BREAK):
                {
                    node = get_break (token, pos, code_error);
                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }
        case (IDENT):
        {
            node = get_expr (token, pos, code_error);
            break;
        }
        default:
        {
            break;
        }
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_if_else (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);
    
    NODE *node = NULL;

    if (CUR_TOK.type == OP && CUR_TOK.data.types_op == IF)
    {
        node = get_if (token, pos, code_error);
        ERR_RET (NULL);

        if (CUR_TOK.type == OP && CUR_TOK.data.types_op == ELSE)
        {
            INCREM;
            node->right = get_if_else (token, pos, code_error);
            ERR_RET (NULL);
        }
    }
    else
    {
        node = get_op (token, pos, code_error);
        ERR_RET (NULL);
    }

    return node;
}

NODE *get_if (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == IF);
    NODE *node = OP_ (IF, NULL);
    INCREM;

    node->left = OP_ (OP_NO, NULL);

    node->left->left  = get_condition (token, pos, code_error);
    ERR_RET (NULL);

    node->left->right = get_op (token, pos, code_error);
    ERR_RET (NULL);

    return node;
}

NODE *get_while (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == WHILE);
    NODE *node = OP_ (WHILE, NULL);
    INCREM;

    node->left  = get_condition (token, pos, code_error);
    ERR_RET (NULL);

    node->right = get_op (token, pos, code_error);
    ERR_RET (NULL);

    return node;
}

NODE *get_stdin (TOKEN *token, op_comand type, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == type);
    NODE *node = OP_ (type, NULL);
    INCREM;
    
    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == OPEN_BRACKET);
    INCREM;

    syntax_assert (CUR_TOK.type == IDENT);
    node->right = IDENT_ (CUR_TOK.data.ident, NULL);
    INCREM;

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == CLOSE_BRACKET);
    INCREM;

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == END_EXPR);
    INCREM;

    return node;
}

NODE *get_break (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == BREAK);
    NODE *node = OP_ (BREAK, NULL);
    INCREM;

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == END_EXPR);
    INCREM;

    return node;
}

NODE *get_condition (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == OPEN_BRACKET);
    INCREM;

    NODE *node = get_compare (token, pos, code_error);
    ERR_RET (NULL);

    while (CUR_TOK.type == OP && (IS_OP (AND) || IS_OP (OR)))
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_compare (token, pos, code_error);
        ERR_RET (NULL);
        
        NODE *node_l = node;

        switch (op)
        {
            case (AND):
            {
                node = AND_ (node_l, node_r);
            }
            case (OR):
            {
                node = OR_ (node_l, node_r);
            }
            default:
            {
                break;
            }
        }
    }

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == CLOSE_BRACKET);
    INCREM;

    return node;
}

NODE *get_compare (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    NODE *node_l = get_add_sub (token, pos, code_error);
    ERR_RET (NULL);

    syntax_assert (CUR_TOK.type == OP);
    op_comand op = CUR_TOK.data.types_op;
    INCREM;

    NODE *node_r = get_add_sub (token, pos, code_error);
    ERR_RET (NULL);

    NODE *node = create_node_op (op, node_l, node_r, NULL, code_error);
    ERR_RET (NULL);

    return node;
}

NODE *get_expr (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    NODE *node_l = IDENT_ (CUR_TOK.data.ident, NULL);
    INCREM;

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == ASSIG)
    INCREM;

    NODE *node_r = get_add_sub (token, pos, code_error);
    ERR_RET (NULL);

    NODE *node = create_node_op (ASSIG, node_l, node_r, NULL, code_error);
    ERR_RET (NULL);

    syntax_assert (CUR_TOK.type == OP && CUR_TOK.data.types_op == END_EXPR);
    INCREM;

    return node;
}

NODE *get_add_sub (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);
    
    NODE *node = get_mul_div (token, pos, code_error);
    ERR_RET (NULL);

    while (CUR_TOK.type == OP && (IS_OP (ADD) || IS_OP (SUB)))
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_mul_div (token, pos, code_error);
        ERR_RET (NULL);

        NODE *node_l = node;

        switch (op)
        {
            case (ADD):
            {
                node = ADD_ (node_l, node_r);
                break;
            }
            case (SUB):
            {
                node = SUB_ (node_l, node_r);
                break;
            }
            case (OP_NO):
            case (MUL):
            case (DIV):
            case (SIN):
            case (COS):
            case (SQRT):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
            default:
            {
                break;
            }
        }
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_mul_div (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    NODE *node = get_pow (token, pos, code_error);
    ERR_RET (NULL);

    while (CUR_TOK.type == OP && (IS_OP (MUL) || IS_OP (DIV)))
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_pow (token, pos, code_error);
        ERR_RET (NULL);

        NODE *node_l = node;

        switch (op)
        {
            case (MUL):
            {
                node = MUL_ (node_l, node_r);
                break;
            }
            case (DIV):
            {
                node = DIV_ (node_l, node_r);
                break;
            }
            case (OP_NO):
            case (ADD):
            case (SUB):
            case (SIN):
            case (COS):
            case (SQRT):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
            default:
            {
                break;
            }
        }
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_pow (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    NODE *node = get_bracket (token, pos, code_error);
    ERR_RET (NULL);

    while (CUR_TOK.type == OP && IS_OP (POW)) 
    {
        INCREM;

        NODE *node_r = get_bracket (token, pos, code_error);
        ERR_RET (NULL);

        NODE *node_l = node;

        node = POW_ (node_l, node_r);
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_bracket (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    if (CUR_TOK.type == OP && IS_OP (OPEN_BRACKET))
    {
        INCREM;

        NODE *node = get_add_sub (token, pos, code_error);
        ERR_RET (NULL);


        syntax_assert (CUR_TOK.type == OP && IS_OP (CLOSE_BRACKET));
        INCREM;

        ERR_RET (NULL);

        return node;
    }

    ERR_RET (NULL);

    return get_trig (token, pos, code_error);
}

NODE *get_trig (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    if (token[*pos].type == OP)
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_bracket (token, pos, code_error);
        ERR_RET (NULL);
        
        switch (op)
        {
            case (SIN):
            {
                return SIN_ (node_r);
            }
            case (COS):
            {
                return COS_ (node_r);
            }
            case (SQRT):
            {
                return SQRT_ (node_r);
            }
            case (OP_NO):
            case (ADD):
            case (SUB):
            case (MUL):
            case (DIV):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
            default:
            {
                break;
            }
        }
    }
    else
    {
        return get_var (token, pos, code_error);
    }

    return NULL;
}

NODE *get_var (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    switch (token[*pos].type)
    {
        case (IDENT):
        {
            return IDENT_ (token[INCREM].data.ident, NULL);
        }
        case (NUM):
        {
            return get_num (token, pos, code_error);
        }
        case (DEF_TYPE):
        case (OP):
        default:
        {
            return NULL;
        }
    }
}

NODE *get_num (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (pos != NULL,   ERR_PTR);

    return NUM_ (token[INCREM].data.value, NULL);
}