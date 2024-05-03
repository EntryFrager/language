#ifndef DSL_H
#define DSL_H

#define IS_NODE_PTR_NULL(ret_value)  if (!node)            {return ret_value;}

#define IF_NUM(node_type, expr)      if (node_type == NUM) {expr; return node;}

#define NUM_(value, parent)   create_node_num   (value,    NULL, NULL, parent, code_error)
#define IDENT_(ident, parent) create_node_ident (ident,  NULL, NULL, parent, code_error)
#define OP_(types_op, parent) create_node_op    (types_op, NULL, NULL, parent, code_error)

#define is_zero(value)  (compare(value, 0) == 0)
#define is_one(value)   (compare(value, 1) == 0)

#define ADD_(node_1, node_2)  create_node_op(ADD,  node_1,        node_2, NULL, code_error)
#define SUB_(node_1, node_2)  create_node_op(SUB,  node_1,        node_2, NULL, code_error)
#define MUL_(node_1, node_2)  create_node_op(MUL,  node_1,        node_2, NULL, code_error)
#define DIV_(node_1, node_2)  create_node_op(DIV,  node_1,        node_2, NULL, code_error)
#define POW_(node_1, deg)     create_node_op(POW,  node_1,        deg,    NULL, code_error)
#define SIN_(node)            create_node_op(SIN,  NUM_(1, NULL), node,   NULL, code_error)
#define COS_(node)            create_node_op(COS,  NUM_(1, NULL), node,   NULL, code_error)
#define SQRT_(node)           create_node_op(SQRT, NUM_(1, NULL), node,   NULL, code_error)
#define LN_(node)             create_node_op(LN,   NUM_(1, NULL), node,   NULL, code_error)
#define AND_(node_1, node_2)  create_node_op(AND,  node_1,        node_2, NULL, code_error)
#define OR_(node_1, node_2)   create_node_op(OR,   node_1,        node_2, NULL, code_error)

#define DIF_L  diff(node->left,  var, code_error)
#define DIF_R  diff(node->right, var, code_error)

#define COPY_L  copy_tree(node->left,  NULL, code_error)
#define COPY_R  copy_tree(node->right, NULL, code_error)

#define L_TYPE   node->left->type
#define R_TYPE   node->right->type

#define L_VALUE  node->left->data.value
#define R_VALUE  node->right->data.value

#define L_RE_HANGING node = hanging_tree(node, node->left,  node->parent, code_error)
#define R_RE_HANGING node = hanging_tree(node, node->right, node->parent, code_error)

#define PRINT_CLOSE_BRACKET(stream)  if (is_bracket) {fprintf(stream, ")"); is_bracket = false;}

#define str_len(str)  (sizeof(str) - 1)

#define CUR_TOK        token[*pos]
#define IS_OP(type)    (CUR_TOK.data.types_op == type)
#define IS_TYPE(type_) (CUR_TOK.type == type_)
#define INCREM         ((*pos)++)

#endif // DSL_H
