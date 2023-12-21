#include "tree.h"

#ifdef DEBUG_TREE

#define DEF_CMD(num, name) name,

static const char *OP_NAME[] = {
    #include "comand.h"
};

#undef DEF_CMD

static int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip);
static void print_tree_dump (NODE *node, FILE *stream);

#define DUMP_LOG(str) fprintf (fp_err, str "\n");
#define DUMP_LOG_PARAM(str, ...) fprintf (fp_err, str "\n", __VA_ARGS__);

void tree_dump_text (TREE *tree, const int *code_error,
                     const char *file_err, const char *func_err,
                     const int line_err)
{
    FILE *fp_err = fopen (tree->info.fp_dump_text_name, "a");

    if (fp_err == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }
    else
    {
        DUMP_LOG ("--------------------------------------------------------------------------------------------------------");

        my_strerr (*code_error, fp_err);

        if (tree == NULL)
        {
            DUMP_LOG_PARAM ("tree[NULL] \"tree\" called from %s(%d) %s", file_err, line_err, func_err);
        }
        else
        {
            DUMP_LOG_PARAM ("tree[%p] \"tree\" called from %s(%d) %s", tree, file_err, line_err, func_err);
            DUMP_LOG ("{");
            DUMP_LOG_PARAM ("\tis_init = %d", tree->is_init);
            DUMP_LOG ("\t{");

            if (tree->root != NULL)
            {
                print_tree_dump (tree->root, fp_err);
            }
            else
            {
                DUMP_LOG ("\t\t*root[NULL]");
            }

            DUMP_LOG ("\t}");
            DUMP_LOG ("}");
        }

        DUMP_LOG ("--------------------------------------------------------------------------------------------------------\n\n");
    }

    if (fclose (fp_err) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }
}

#undef DUMP_LOG
#undef DUMP_LOG_PARAM

void print_tree_dump (NODE *node, FILE *stream)
{
    IS_NODE_PTR_NULL ();

    switch (node->type)
    {
        case (NUM):
        {
            fprintf (stream, "\t\t*node[%p] = %lg;\n", node, node->data.value);
            break;
        }
        case (OP):
        {
            fprintf (stream, "\t\t*node[%p] = %s;\n", node, OP_NAME[node->data.types_op]);
            break;
        }
        case (IDENT):
        {
            fprintf (stream, "\t\t*node[%p] = %s;\n", node, node->data.ident);
            break;
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }

    print_tree_dump (node->left, stream);
    print_tree_dump (node->right, stream);
}

#define DUMP_DOT(str) fprintf (fp_dot, str "\n");
#define DUMP_DOT_PARAM(str, ...) fprintf (fp_dot, str "\n", __VA_ARGS__);

void tree_dump_graph_viz (TREE *tree, const char *file_err, 
                          const char *func_err, const int line_err)
{
    FILE *fp_dot = fopen (tree->info.fp_dot_name, "w+");

    if (fp_dot == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }
    else
    {
        if (tree != NULL)
        {
            DUMP_DOT ("digraph List {");
            DUMP_DOT ("\trankdir = HR;");
            DUMP_DOT ("\tbgcolor = " BACK_GROUND_COLOR ";");

            if (tree->root != NULL)
            {
                create_node_dot (tree->root, fp_dot, -1, 0);
            }

            DUMP_DOT_PARAM ("\tlabel = \"tree_dump from function %s, Tree/%s:%d\";}\n", func_err, file_err, line_err);
        }
    }

    if (fclose (fp_dot) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }

    const char comand[] = "dot -Tsvg ../DEBUG/dump.dot -o ../DEBUG/dot.svg";

    system (comand);

    int code_error = 0;

    tree_dump_html (tree, &code_error);

    my_strerr (code_error, stdout);
}

#undef DUMP_DOT
#undef DUMP_DOT_PARAM

#undef DEBUG_TREE

#ifdef DEBUG_TREE
    #define DUMP_DOT(specifier, value) fprintf (stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{idx: %p | value: " specifier " | left: %p | right: %p | parent: %p}\"];\n",              \
                ip, color, node, value, node->left, node->right, node->parent);
#else
    #define DUMP_DOT(specifier, value) fprintf (stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{" specifier "}\"];\n", ip, color, value);
#endif

int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip)
{
    IS_NODE_PTR_NULL (ip - 1);

    const char *color = BLACK_COLOR;

    switch (node->type)
    {
        case (NUM):
        {
            color = BLUE_COLOR;
            DUMP_DOT ("%lg", node->data.value);
            break;
        }
        case (OP):
        {
            color = PURPLE_COLOR; 
            DUMP_DOT ("%s", OP_NAME[node->data.types_op]);
            break;
        }
        case (IDENT):
        {
            color = LIGHT_GREEN_COLOR; 
            DUMP_DOT ("%s", node->data.ident);
            break;
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }

    if (ip > 0)
    {
        fprintf (stream, "\tnode%d -> node%d [color = %s];\n", ip_parent, ip, color);
    }

    ip_parent = ip;

    ip = create_node_dot (node->left, stream, ip_parent, ip + 1);
    ip = create_node_dot (node->right, stream, ip_parent, ip + 1);

    return ip;
}

void tree_dump_html (TREE *tree, int *code_error)
{
    FOPEN_ (fp_dot, tree->info.fp_image, "r");
    FOPEN_ (fp_html_dot, tree->info.fp_name_html, "w+");
    
    size_t size_dot = get_file_size (fp_dot, code_error);

    if (*code_error != ERR_NO)
    {
        return;
    }

    char *data_dot = (char *) calloc (size_dot, sizeof (char));
    my_assert (data_dot != NULL, ERR_MEM)

    fread (data_dot, sizeof (char), size_dot, fp_dot);

    fprintf (fp_html_dot, "%s", data_dot);
    
    FCLOSE_ (fp_dot);
    FCLOSE_ (fp_html_dot);

    free (data_dot);
}

#endif