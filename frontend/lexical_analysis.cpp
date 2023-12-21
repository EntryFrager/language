#include "input.h"

static size_t get_n_tokens   (char *str, int *code_error);
static op_comand is_key_word (char **str, int *code_error);

void get_token (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    char *str = tree->info.buf;

    tree->n_token = get_n_tokens (str, code_error) + 1;
    ERR_RET ();

    tree->token = (TOKEN *) calloc (tree->n_token, sizeof (TOKEN));
    my_assert (tree->token != NULL, ERR_MEM);

    op_comand op = OP_NO;
    size_t pos = 0;

    while (*str != '\0')
    {
        if ((op = is_key_word (&str, code_error)) != OP_NO)
        {
            tree->token[pos].type = OP;
            tree->token[pos].data.types_op = op;
        }
        else if (isdigit (*str) || *str == '-')
        {
            tree->token[pos].type = NUM;

            size_t n_read = 0;
            sscanf (str, "%lf%n", &tree->token[pos].data.value, &n_read);
            str += n_read;
        }
        else if (isalpha (*str))
        {
            tree->n_ident++;
            tree->token[pos].type = IDENT;
            tree->token[pos].data.ident = read_ident (&str, code_error);
            ERR_RET ();
        }

        pos++;
    }

    tree->token[tree->n_token - 1].type = DEF_TYPE;
}

size_t get_n_tokens (char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
    size_t n_tokens = 0;

    while (*str != '\0')
    {
        if (is_key_word (&str, code_error) != OP_NO)
        {
            n_tokens++;
        }
        else if (isdigit (*str) || isalpha (*str) || *str == '-')
        {
            n_tokens++;

            while (isdigit (*str) || isalpha (*str))
            {
                str++;
            }
        }
    }
    
    return n_tokens;
}

#define DEF_CMD(func_code, name)                    \
    if (strncmp (*str, name, str_len (name)) == 0)  \
    {                                               \
        *str += str_len (name);                     \
        return func_code;                           \
    } else                                          \

op_comand is_key_word (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    #include "../include/comand.h"
    {}
 
    return OP_NO;
}

#undef DEF_CMD