#ifndef DUMP_CPP
#define DUMP_CPP

#include "tree.h"

#ifdef DEBUG_TREE

#define DEF_CMD(num, name) name,

static const char *OP_NAME[] = {
    #include "comand.h"

    "CALL_FUNC",
    "no key word"
};

#undef DEF_CMD

static const char *DEBUG_FOLDER              = "../debug";
static const char *TREE_DEBUG_FOLDER         = "../debug/tree_debug";
static const char *TREE_FP_ERR_NAME          = "../debug/tree_debug/tree_debug.txt";
static const char *TREE_FP_DOT_NAME          = "../debug/tree_debug/dump.dot";
static const char *TREE_FP_HTML_DOT_NAME     = "../debug/tree_debug/dot.html";
static const char *TREE_FP_IMAGE             = "../debug/tree_debug/dot.svg";

static const char *CMD_COMPILE_TREE_DOT_FILE = "dot -Tsvg ../debug/tree_debug/dump.dot -o ../debug/tree_debug/dot.svg";

static void print_tree_dump (Node *node, FILE *fp_err);
static int  create_node_dot (Node *node, FILE *stream, int ip_parent, int ip);
static void tree_dump_html  (int *code_error);

#define DUMP_LOG(str) fprintf(fp_err, str "\n");
#define DUMP_LOG_PARAM(str, ...) fprintf(fp_err, str "\n", __VA_ARGS__);

void tree_dump_text (Tree *tree, int *code_error,
                     const char *file_err, const char *func_err,
                     const int line_err)
{
    create_folder(DEBUG_FOLDER, code_error);
    create_folder(TREE_DEBUG_FOLDER, code_error);

    fopen_(fp_err, TREE_FP_ERR_NAME, "a");

    if (fp_err == NULL)
    {
        my_strerr(*code_error, stderr);
    }
    else
    {
        DUMP_LOG("--------------------------------------------------------------------------------------------------------");

        my_strerr(*code_error, fp_err);

        if (tree == NULL)
        {
            DUMP_LOG_PARAM("tree[NULL] \"tree\" called from %s(%d) %s", file_err, line_err, func_err);
        }
        else
        {
            DUMP_LOG_PARAM("tree[%p] \"tree\" called from %s(%d) %s", tree, file_err, line_err, func_err);
            DUMP_LOG("{");
            DUMP_LOG_PARAM("\tis_init = %d", tree->is_init);
            DUMP_LOG("\t{");

            if (tree->root != NULL)
            {
                print_tree_dump(tree->root, fp_err);
            }
            else
            {
                DUMP_LOG("\t\t*root[NULL]");
            }

            DUMP_LOG("\t}");
            DUMP_LOG("}");
        }

        DUMP_LOG("--------------------------------------------------------------------------------------------------------\n\n");
    }

    fclose(fp_err);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

void print_tree_dump (Node *node, FILE *fp_err)
{
    IS_NODE_PTR_NULL();

    switch (node->type)
    {
        case (NUM):
        {
            DUMP_LOG_PARAM("\t\t*node[%p] = %lg;\n", node, node->data.value);
            break;
        }
        case (OP):
        {
            DUMP_LOG_PARAM("\t\t*node[%p] = %s;\n", node, OP_NAME[node->data.types_op]);
            break;
        }
        case (IDENT):
        {
            DUMP_LOG_PARAM("\t\t*node[%p] = %d;\n", node, node->data.ident);
            break;
        }
        case (IDENT_FUNC):
        {
            DUMP_LOG_PARAM("\t\t*node[%p] = %d;\n", node, node->data.ident);
            break;
        }
        case (PARAM):
        {
            DUMP_LOG_PARAM("\t\t*node[%p] = %d;\n", node, node->data.ident);
            break;
        }
        default:
        {
            break;
        }
    }

    print_tree_dump(node->left, fp_err);
    print_tree_dump(node->right, fp_err);
}

#undef DUMP_LOG
#undef DUMP_LOG_PARAM

#define DUMP_DOT(str) fprintf(fp_dot, str "\n");
#define DUMP_DOT_PARAM(str, ...) fprintf(fp_dot, str "\n", __VA_ARGS__);

void tree_dump_graph_viz (Tree *tree, int *code_error, const char *file_err,
                          const char *func_err, const int line_err)
{
    fopen_(fp_dot, TREE_FP_DOT_NAME, "w+");

    if (fp_dot != NULL)
    {
        if (tree != NULL)
        {
            DUMP_DOT("digraph List {");
            DUMP_DOT("\trankdir = HR;");
            DUMP_DOT("\tbgcolor = " BACK_GROUND_COLOR ";");

            if (tree->root != NULL)
            {
                create_node_dot(tree->root, fp_dot, -1, 0);
            }

            DUMP_DOT_PARAM("\tlabel = \"tree_dump from function %s, Tree/%s:%d\";}\n", func_err, file_err, line_err);
        }
    }

    fclose(fp_dot);

    my_assert(system(CMD_COMPILE_TREE_DOT_FILE) != -1, ERR_SYSTEM);

    tree_dump_html(code_error);

    my_strerr(*code_error, stderr);
}

#undef DUMP_DOT
#undef DUMP_DOT_PARAM

#undef DEBUG_TREE

#ifdef DEBUG_TREE
    #define DUMP_DOT(specifier, value) fprintf(stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{idx: %p | value: " specifier " | left: %p | right: %p | parent: %p}\"];\n",              \
                 ip, color, node, value, node->left, node->right, node->parent);
#else
    #define DUMP_DOT(specifier, value) fprintf(stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{" specifier "}\"];\n", ip, color, value);
#endif

int create_node_dot (Node *node, FILE *stream, int ip_parent, int ip)
{
    IS_NODE_PTR_NULL(ip - 1);

    const char *color = BLACK_COLOR;

    switch (node->type)
    {
        case (NUM):
        {
            color = BLUE_COLOR;
            DUMP_DOT("%lg", node->data.value);
            break;
        }
        case (OP):
        {
            switch (node->data.types_op)
            {
                case (OP_NO):
                {
                    color = WHITE_COLOR;
                    break;
                }
                case (END_EXPR):
                {
                    color = DARK_GREEN_COLOR;
                    break;
                }
                default:
                {
                    color = PURPLE_COLOR;
                    break;
                }
            }

            DUMP_DOT("%s", OP_NAME[node->data.types_op]);
            break;
        }
        case (IDENT):
        {
            color = LIGHT_GREEN_COLOR;
            DUMP_DOT("%d", node->data.ident);
            break;
        }
        case (IDENT_FUNC):
        {
            color = RED_COLOR;
            DUMP_DOT("%d", node->data.ident);
            break;
        }
        case (PARAM):
        {
            color = MAGENTA;
            DUMP_DOT("%d", node->data.ident);
            break;
        }
        default:
        {
            break;
        }
    }

    if (ip > 0)
    {
        fprintf(stream, "\tnode%d -> node%d [color = %s];\n", ip_parent, ip, color);
    }

    ip_parent = ip;

    ip = create_node_dot(node->left, stream, ip_parent, ip + 1);
    ip = create_node_dot(node->right, stream, ip_parent, ip + 1);

    return ip;
}

#pragma GCC diagnostic pop

void tree_dump_html (int *code_error)
{
    fopen_(fp_dot, TREE_FP_IMAGE, "r + b");
    fopen_(fp_html_dot, TREE_FP_HTML_DOT_NAME, "w+");

    size_t size_dot = get_file_size(fp_dot, code_error);

    if (*code_error != ERR_NO)
    {
        return;
    }

    calloc_(data_dot, char *, size_dot, sizeof(char))

    fread(data_dot, sizeof (char), size_dot, fp_dot);

    fprintf(fp_html_dot, "%s", data_dot);

    fclose_(fp_dot);
    fclose_(fp_html_dot);

    free(data_dot);
}

#endif

#endif // DUMP_CPP
