#ifndef PARSING_CPP
#define PARSING_CPP

#include "../inc/input.h"
#include "../inc/syntax_error.h"

static int n_scope = 0;

static Node *get_funcs        (Token *token, size_t *pos, TableName *table_name, int *code_error);
static Node *get_func         (Token *token, size_t *pos, TableName *table_name, int *code_error);
static Node *get_params       (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_param        (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_body_scope   (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_instructions (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_node         (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_op           (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_if_else      (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error);
static Node *get_if           (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error);
static Node *get_while        (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_stdin        (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error);
static Node *get_abc          (Token *token, size_t *pos,                            op_command op_type, int *code_error);  // this function creates nodes like abort, break and continue
static Node *get_return       (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_condition    (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_compare      (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_call_func    (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_expr         (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_add_sub      (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_mul_div      (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_pow          (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_bracket      (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_trig         (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_var          (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error);
static Node *get_num          (Token *token, size_t *pos, int *code_error);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

Node *get_code (Tree *tree, int *code_error)
{
    assert_tree(tree);

    size_t pos = 0;

    add_table_name(&tree->table_name, func_scope_table_name, code_error);
    n_scope++;

    Node *node = get_funcs(tree->token, &pos, &tree->table_name, code_error);

    syntax_assert(tree->token[pos].type == DEF_TYPE, UNKNOWN_ERROR);

    ERR_RET(NULL);

    return node;
}

Node *get_funcs (Token *token, size_t *pos, TableName *table_name, int *code_error)
{
    my_assert(token      != NULL, ERR_PTR);
    my_assert(pos        != NULL, ERR_PTR);

    Node *node = OP_(END_EXPR, NULL);

    if (CUR_TOK.type != DEF_TYPE)
    {
        node->left = get_func(token, pos, table_name, code_error);
        ERR_RET(NULL);

        n_scope++;

        node->right = get_funcs(token, pos, table_name, code_error);
        ERR_RET(NULL);
    }

    return node;
}

Node *get_func (Token *token, size_t *pos, TableName *table_name, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    syntax_assert(IS_TYPE(OP) && IS_OP(FUNC), NOT_FUNC_DEF);
    INCREM;

    syntax_assert(IS_TYPE(IDENT_FUNC), NOT_IDENT_FUNC);

    add_table_name(table_name, CUR_TOK.data.ident, code_error);

    Node *node = IDENT_FUNC_(CUR_TOK.data.ident, NULL);
    add_table_name_elem(table_name->scope_table_name[0], CUR_TOK.data.ident, IDENT_FUNC, code_error);
    INCREM;

    node->left = get_params(token, pos, table_name->scope_table_name[n_scope], code_error);
    ERR_RET(NULL);

    node->right = get_body_scope(token, pos, table_name->scope_table_name[n_scope], code_error);
    ERR_RET(NULL);

    table_name->scope_table_name[n_scope]->n_elem = table_name->scope_table_name[n_scope]->cur_elem;

    return node;
}

Node *get_params (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    syntax_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACKET), NOT_OPEN_BRACKET);
    INCREM;

    Node *node = get_param(token, pos, cur_scope, code_error);

    syntax_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), NOT_CLOSE_BRACKET);
    INCREM;

    return node;
}

Node *get_param (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (!IS_TYPE(IDENT))
    {
        return NULL;
    }

    Node *node = PARAM_(CUR_TOK.data.ident, NULL);
    add_table_name_elem(cur_scope, CUR_TOK.data.ident, IDENT, code_error);
    INCREM;

    if (IS_TYPE(OP) && IS_OP(COMMA))
    {
        INCREM;
        node->left = get_param(token, pos, cur_scope, code_error);
    }

    return node;
}

Node *get_body_scope (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    syntax_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACE), NOT_OPEN_BRACE);
    INCREM;

    Node *node = get_instructions(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_instructions(Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (IS_TYPE(OP) && IS_OP(CLOSE_BRACE))
    {
        INCREM;
        return NULL;
    }

    Node *node = OP_(END_EXPR, NULL);

    node->left = get_node(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    node->right = get_instructions(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_node (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = NULL;

    switch (CUR_TOK.type)
    {
        case (OP):
        {
            node = get_op(token, pos, cur_scope, code_error);

            break;
        }
        case (IDENT):
        {
            node = get_expr(token, pos, cur_scope, code_error);
            break;
        }
        case (CALL_FUNC):
        {
            node = get_call_func(token, pos, cur_scope, code_error);
            break;
        }
        default:
        {
            break;
        }
    }

    ERR_RET(NULL);

    return node;
}

Node *get_op (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = NULL;

    switch (CUR_TOK.data.types_op)
    {
        case (IF):
        {
            node = get_if_else(token, pos, cur_scope, IF, code_error);
            break;
        }
        case (WHILE):
        {
            node = get_while(token, pos, cur_scope, code_error);
            break;
        }
        case (OPEN_BRACE):
        {
            node = get_body_scope(token, pos, cur_scope, code_error);
            break;
        }
        case (PRINT):
        case (INPUT):
        {
            node = get_stdin(token, pos, cur_scope, CUR_TOK.data.types_op, code_error);
            break;
        }
        case (BREAK):
        case (CONTINUE):
        case (ABORT):
        {
            node = get_abc(token, pos, CUR_TOK.data.types_op, code_error);
            break;
        }
        case (RETURN):
        {
            node = get_return(token, pos, cur_scope, code_error);
            break;
        }
        default:
        {
            break;
        }
    }

    ERR_RET(NULL);

    return node;
}

Node *get_if_else (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(op_type, NULL);
    INCREM;

    node->left = get_if(token, pos, cur_scope, op_type, code_error);
    ERR_RET(NULL);

    if (IS_TYPE(OP) && (IS_OP(ELSE_IF) || IS_OP(ELSE)))
    {
        node->right = get_if_else(token, pos, cur_scope, CUR_TOK.data.types_op, code_error);
        ERR_RET(NULL);
    }

    return node;
}

Node *get_if (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(OP_NO, NULL);

    if (op_type != ELSE)
    {
        node->left = get_condition(token, pos, cur_scope, code_error);
        ERR_RET(NULL);
    }

    node->right = get_body_scope(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_while (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(WHILE, NULL);
    INCREM;

    node->left = get_condition(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    node->right = get_body_scope(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_stdin (Token *token, size_t *pos, ScopeTableName *cur_scope, op_command op_type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(op_type, NULL);
    INCREM;

    node->left = get_params(token, pos, cur_scope, code_error);

    syntax_assert(IS_TYPE(OP) && IS_OP(END_EXPR), NOT_END_EXPR);
    INCREM;

    return node;
}

Node *get_abc (Token *token, size_t *pos, op_command op_type, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(op_type, NULL);
    INCREM;

    syntax_assert(IS_TYPE(OP) && IS_OP(END_EXPR), NOT_END_EXPR);
    INCREM;

    return node;
}

Node *get_return (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = OP_(RETURN, NULL);
    INCREM;

    node->left = get_add_sub(token, pos, cur_scope, code_error);

    syntax_assert(IS_TYPE(OP) && IS_OP(END_EXPR), NOT_END_EXPR);
    INCREM;

    return node;
}

Node *get_condition (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    syntax_assert(IS_TYPE(OP) && IS_OP(OPEN_BRACKET), NOT_OPEN_BRACKET);
    INCREM;

    Node *node = get_compare(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(AND) || IS_OP(OR)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_compare(token, pos, cur_scope, code_error);
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

    syntax_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), NOT_CLOSE_BRACKET);
    INCREM;

    return node;
}

Node *get_compare (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node_l = get_add_sub(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    syntax_assert(IS_TYPE(OP), NOT_OP);
    op_command op = CUR_TOK.data.types_op;
    INCREM;

    Node *node_r = get_add_sub(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    Node *node = create_node_op(op, node_l, node_r, NULL, code_error);
    ERR_RET(NULL);

    return node;
}

Node *get_call_func (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = CALL_FUNC_(CUR_TOK.data.ident, NULL);
    add_table_name_elem(cur_scope, CUR_TOK.data.ident, CALL_FUNC, code_error);
    INCREM;

    node->left = get_params(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    syntax_assert(IS_TYPE(OP) && IS_OP(END_EXPR), NOT_END_EXPR);
    INCREM;

    return node;
}

Node *get_expr (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node_l = IDENT_(CUR_TOK.data.ident, NULL);
    add_table_name_elem(cur_scope, CUR_TOK.data.ident, IDENT, code_error);
    INCREM;

    syntax_assert(IS_TYPE(OP) && IS_OP(ASSIG), NOT_ASSIG);
    INCREM;

    Node *node_r = get_add_sub(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    Node *node = create_node_op(ASSIG, node_l, node_r, NULL, code_error);
    ERR_RET(NULL);

    syntax_assert(IS_TYPE(OP) && IS_OP(END_EXPR), NOT_END_EXPR);
    INCREM;

    return node;
}

Node *get_add_sub (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_mul_div(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(ADD) || IS_OP(SUB)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_mul_div(token, pos, cur_scope, code_error);
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

Node *get_mul_div (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_pow(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && (IS_OP(MUL) || IS_OP(DIV)))
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_pow(token, pos, cur_scope, code_error);
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

Node *get_pow (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    Node *node = get_bracket(token, pos, cur_scope, code_error);
    ERR_RET(NULL);

    while (IS_TYPE(OP) && IS_OP(POW))
    {
        INCREM;

        Node *node_r = get_bracket(token, pos, cur_scope, code_error);
        ERR_RET(NULL);

        Node *node_l = node;

        node = POW_(node_l, node_r);
    }

    ERR_RET(NULL);

    return node;
}

Node *get_bracket (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (IS_TYPE(OP) && IS_OP(OPEN_BRACKET))
    {
        INCREM;

        Node *node = get_add_sub(token, pos, cur_scope, code_error);
        ERR_RET(NULL);

        syntax_assert(IS_TYPE(OP) && IS_OP(CLOSE_BRACKET), NOT_CLOSE_BRACKET);
        INCREM;

        ERR_RET(NULL);

        return node;
    }

    ERR_RET(NULL);

    return get_trig(token, pos, cur_scope, code_error);
}

Node *get_trig (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    if (token[*pos].type == OP)
    {
        op_command op = CUR_TOK.data.types_op;
        INCREM;

        Node *node_r = get_bracket(token, pos, cur_scope, code_error);
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
        return get_var(token, pos, cur_scope, code_error);
    }

    return NULL;
}

Node *get_var (Token *token, size_t *pos, ScopeTableName *cur_scope, int *code_error)
{
    my_assert(token != NULL, ERR_PTR);
    my_assert(pos   != NULL, ERR_PTR);

    switch (token[*pos].type)
    {
        case (IDENT):
        {
            add_table_name_elem(cur_scope, CUR_TOK.data.ident, IDENT, code_error);
            return IDENT_(token[INCREM].data.ident, NULL);
        }
        case (CALL_FUNC):
        {
            Node *node = get_call_func(token, pos, cur_scope, code_error);
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
