#ifndef PARSING_CPP
#define PARSING_CPP

#include "../inc/input.h"

static Node *get_funcs       (Token *token, size_t *pos, int *code_error);
static Node *get_func        (Token *token, size_t *pos, int *code_error);
static Node *get_params      (Token *token, size_t *pos, int *code_error);
static Node *get_param       (Token *token, size_t *pos, int *code_error);
static Node *get_body_scope  (Token *token, size_t *pos, int *code_error);
static Node *get_operators   (Token *token, size_t *pos, int *code_error);
static Node *get_op          (Token *token, size_t *pos, int *code_error);
static Node *get_if_else     (Token *token, size_t *pos, int *code_error);
static Node *get_else_if     (Token *token, size_t *pos, int *code_error);
static Node *get_if          (Token *token, size_t *pos, int *code_error);
static Node *get_while       (Token *token, size_t *pos, int *code_error);
static Node *get_stdin       (Token *token, size_t *pos, op_command type, int *code_error);
static Node *get_bc          (Token *token, size_t *pos, op_command type, int *code_error);
static Node *get_return      (Token *token, size_t *pos, int *code_error);
static Node *get_condition   (Token *token, size_t *pos, int *code_error);
static Node *get_compare     (Token *token, size_t *pos, int *code_error);
static Node *get_call_func   (Token *token, size_t *pos, int *code_error);
static Node *get_expr        (Token *token, size_t *pos, int *code_error);
static Node *get_add_sub     (Token *token, size_t *pos, int *code_error);
static Node *get_mul_div     (Token *token, size_t *pos, int *code_error);
static Node *get_pow         (Token *token, size_t *pos, int *code_error);
static Node *get_bracket     (Token *token, size_t *pos, int *code_error);
static Node *get_trig        (Token *token, size_t *pos, int *code_error);
static Node *get_var         (Token *token, size_t *pos, int *code_error);
static Node *get_num         (Token *token, size_t *pos, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

Node *get_code (Token *token, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);

    size_t pos = 0;

    Node *node = get_funcs(token, &pos, code_error);

    my_assert(token[pos].type == DEF_TYPE, SYNTAX_ERROR);

    ERR_RET(NULL);

    return node;
}

Node *get_funcs (Token *token, size_t *pos, int *code_error)
{
    my_assert(token      != NULL, ERR_PTR);
    my_assert(pos        != NULL, ERR_PTR);

    Node *node = OP_(END_EXPR, NULL);

    if (CUR_TOK.type != DEF_TYPE)
    {
        node->left  = get_func(token, pos, code_error);
        ERR_RET(NULL);

        node->right = get_funcs(token, pos, code_error);
        ERR_RET(NULL);
    }

    return node;
}

Node *get_func (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    my_assert(IS_TYPE(OP) && IS_OP(FUNC), SYNTAX_ERROR);
    INCREM;

    my_assert(IS_TYPE(IDENT_FUNC), SYNTAX_ERROR);
    Node *node = create_node(IDENT_FUNC, NULL, NULL, NULL, code_error);
    node->data.ident = CUR_TOK.data.ident;
    INCREM;

    node->left = get_params(token, pos, code_error);
    ERR_RET(NULL);

    node->right = get_body_scope(token, pos, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_params (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    my_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACKET), SYNTAX_ERROR);
    INCREM;

    Node *node = get_param(token, pos, code_error);

    my_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_param (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (!IS_TYPE(IDENT))
    {
        return NULL;
    }

    Node *node = IDENT_(CUR_TOK.data.ident, NULL);
    INCREM;

    if (IS_TYPE(OP) && IS_OP(COMMA))
    {
        INCREM;
        node->left = get_param(token, pos, code_error);
    }

    return node;
}

Node *get_body_scope (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    my_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACE), SYNTAX_ERROR);
    INCREM;

    Node *node = get_operators(token, pos, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_operators(Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (IS_TYPE(OP) && IS_OP(CLOSE_BRACE))
    {
        INCREM;
        return NULL;
    }

    Node *node = OP_(END_EXPR, NULL);

    node->left = get_op(token, pos, code_error);
    ERR_RET(NULL);

    node->right = get_operators(token, pos, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_op (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = NULL;

    switch (CUR_TOK.type)
    {
        case (OP):
        {
            switch (CUR_TOK.data.types_op)
            {
                case (IF):
                {
                    node = get_if_else(token, pos, code_error);
                    break;
                }
                case (WHILE):
                {
                    node = get_while(token, pos, code_error);
                    break;
                }
                case (OPEN_BRACE):
                {
                    node = get_body_scope(token, pos, code_error);
                    break;
                }
                case (PRINT):
                {
                    node = get_stdin(token, pos, PRINT, code_error);
                    break;
                }
                case (INPUT):
                {
                    node = get_stdin(token, pos, INPUT, code_error);
                    break;
                }
                case (BREAK):
                {
                    node = get_bc(token, pos, BREAK, code_error);
                    break;
                }
                case (CONTINUE):
                {
                    node = get_bc(token, pos, CONTINUE, code_error);
                    break;
                }
                case (RETURN):
                {
                    node = get_return(token, pos, code_error);
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
            node = get_expr(token, pos, code_error);
            break;
        }
        case (CALL_FUNC):
        {
            node = get_call_func(token, pos, code_error);
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

Node *get_if_else (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(IF, NULL);
    INCREM;

    node->left = get_if(token, pos, code_error);
    ERR_RET(NULL);

    if (IS_TYPE(OP) && IS_OP(ELSE))
    {
        INCREM;
        node->right = get_else_if(token, pos, code_error);
        ERR_RET(NULL);
    }

    return node;
}

Node *get_else_if (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = NULL;

    if (IS_TYPE(OP) && IS_OP(IF))
    {
        node = get_if_else(token, pos, code_error);
        ERR_RET(NULL);
    }
    else
    {
        node = get_body_scope(token, pos, code_error);
        ERR_RET(NULL);
    }

    return node;
}

Node *get_if (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(OP_NO, NULL);

    node->left = get_condition(token, pos, code_error);
    ERR_RET(NULL);

    node->right = get_body_scope(token, pos, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_while (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(WHILE, NULL);
    INCREM;

    node->left = get_condition(token, pos, code_error);
    ERR_RET(NULL);

    node->right = get_body_scope(token, pos, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_stdin (Token *token, size_t *pos, op_command type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(type, NULL);
    INCREM;

    node->left = get_params(token, pos, code_error);

    my_assert(IS_TYPE(OP) && IS_OP(END_EXPR), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_bc (Token *token, size_t *pos, op_command type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(type, NULL);
    INCREM;

    my_assert(IS_TYPE(OP) && IS_OP(END_EXPR), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_return (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(RETURN, NULL);
    INCREM;

    node->left = get_add_sub(token, pos, code_error);

    my_assert(IS_TYPE(OP) && IS_OP(END_EXPR), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_condition (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    my_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACKET), SYNTAX_ERROR);
    INCREM;

    Node *node = get_compare(token, pos, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(AND) || IS_OP(OR)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_compare(token, pos, code_error);
        ERR_RET(NULL);

        Node *node_l = node;

        switch (op)
        {
            case (AND):
            {
                node = AND_(node_l, node_r);
                break;
            }
            case (OR):
            {
                node = OR_(node_l, node_r);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    my_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_compare (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node_l = get_add_sub(token, pos, code_error);
    ERR_RET(NULL);

    my_assert(IS_TYPE(OP), SYNTAX_ERROR);
    op_command op = CUR_TOK.data.types_op;
    INCREM;

    Node *node_r = get_add_sub(token, pos, code_error);
    ERR_RET(NULL);

    Node *node = create_node_op(op, node_l, node_r, NULL, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_call_func (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = create_node(CALL_FUNC, NULL, NULL, NULL, code_error);
    ERR_RET(NULL)

    node->right = IDENT_(CUR_TOK.data.ident, NULL);
    ERR_RET(NULL);
    INCREM;

    node->left = get_params(token, pos, code_error);
    ERR_RET(NULL);

    my_assert(IS_TYPE(OP) && IS_OP(END_EXPR), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_expr (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node_l = IDENT_(CUR_TOK.data.ident, NULL);
    INCREM;

    my_assert(IS_TYPE(OP) && IS_OP(ASSIG), SYNTAX_ERROR);
    INCREM;

    Node *node_r = get_add_sub(token, pos, code_error);
    ERR_RET(NULL);

    Node *node = create_node_op(ASSIG, node_l, node_r, NULL, code_error);
    ERR_RET(NULL);

    my_assert(IS_TYPE(OP) && IS_OP(END_EXPR), SYNTAX_ERROR);
    INCREM;

    return node;
}

Node *get_add_sub (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_mul_div(token, pos, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(ADD) || IS_OP(SUB)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_mul_div(token, pos, code_error);
        ERR_RET(NULL);

        Node *node_l = node;

        switch (op)
        {
            case (ADD):
            {
                node = ADD_(node_l, node_r);
                break;
            }
            case (SUB):
            {
                node = SUB_(node_l, node_r);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    ERR_RET(NULL);

    return node;
}

Node *get_mul_div (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_pow(token, pos, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(MUL) || IS_OP(DIV)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_pow(token, pos, code_error);
        ERR_RET(NULL);

        Node *node_l = node;

        switch (op)
        {
            case (MUL):
            {
                node = MUL_(node_l, node_r);
                break;
            }
            case (DIV):
            {
                node = DIV_(node_l, node_r);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    ERR_RET(NULL);

    return node;
}

Node *get_pow (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_bracket(token, pos, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && IS_OP(POW))
    {
        INCREM;

        Node *node_r = get_bracket(token, pos, code_error);
        ERR_RET(NULL);

        Node *node_l = node;

        node = POW_(node_l, node_r);
    }

    ERR_RET(NULL);

    return node;
}

Node *get_bracket (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (IS_TYPE(OP) && IS_OP(OPEN_BRACKET))
    {
        INCREM;

        Node *node = get_add_sub(token, pos, code_error);
        ERR_RET(NULL);

        my_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), SYNTAX_ERROR);
        INCREM;

        ERR_RET(NULL);

        return node;
    }

    ERR_RET(NULL);

    return get_trig(token, pos, code_error);
}

Node *get_trig (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (token[*pos].type == OP)
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_bracket(token, pos, code_error);
        ERR_RET(NULL);

        switch (op)
        {
            case (SIN):
            {
                return SIN_(node_r);
            }
            case (COS):
            {
                return COS_(node_r);
            }
            case (SQRT):
            {
                return SQRT_(node_r);
            }
            default:
            {
                break;
            }
        }
    }
    else
    {
        return get_var(token, pos, code_error);
    }

    return NULL;
}

Node *get_var (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    switch (token[*pos].type)
    {
        case (IDENT):
        {
            return IDENT_(token[INCREM].data.ident, NULL);
        }
        case (CALL_FUNC):
        {
            Node *node = get_call_func(token, pos, code_error);
            (*pos)--;

            return node;
        }
        case (NUM):
        {
            return get_num(token, pos, code_error);
        }
        default:
        {
            return NULL;
        }
    }
}

Node *get_num (Token *token, size_t *pos, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    return NUM_(token[INCREM].data.value, NULL);
}

#pragma GCC diagnostic pop

#endif // PARSING_CPP
