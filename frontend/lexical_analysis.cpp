#include "input.h"

static size_t    get_n_tokens   (char *str, int *code_error);
static size_t    token_analysis (TOKEN *token, size_t n_token, char *str, int *code_error);
static op_comand is_key_word    (char **str, int *code_error);

void get_token (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    char *str = tree->info.buf;

    tree->n_token = get_n_tokens (str, code_error);
    ERR_RET ();

    tree->token = (TOKEN *) calloc (tree->n_token + 1, sizeof (TOKEN));
    my_assert (tree->token != NULL, ERR_MEM);

    tree->n_ident = token_analysis (tree->token, tree->n_token, str, code_error);
    ERR_RET ();
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
        else if (isdigit (*str) || isalpha (*str) || *str == '-' || *str == '_')
        {
            n_tokens++;

            while (isdigit (*str) || isalpha (*str) || *str == '_')
            {
                str++;
            }
        }
    }
    
    return n_tokens;
}

size_t token_analysis (TOKEN *token, size_t n_token, char *str, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    my_assert (str   != NULL, ERR_PTR);

    op_comand op = OP_NO;
    size_t n_ident = 0;

    for (size_t pos = 0; pos < n_token; pos++)
    {
        if ((op = is_key_word (&str, code_error)) != OP_NO)
        {
            token[pos].type = OP;
            token[pos].data.types_op = op;
        }
        else if (isdigit (*str) || *str == '-')
        {
            token[pos].type = NUM;

            size_t n_read = 0;
            sscanf (str, "%lf%n", &token[pos].data.value, &n_read);

            str += n_read;
        }
        else if (isalpha (*str) || *str == '_')
        {
            token[pos].type = IDENT;

            n_ident++;
            token[pos].data.ident = read_ident (&str, code_error);
            ERR_RET (n_ident);
        }
    }

    syntax_assert (*str == '\0');

    token[n_token].type = DEF_TYPE;

    return n_ident;
}

#define DEF_CMD(func_code, name)                    \
    if (strncmp (*str, name, str_len (name)) == 0)  \
    {                                               \
        *str += str_len (name);                     \
        return func_code;                           \
    } else

op_comand is_key_word (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    #include "../include/comand.h"
    {}
 
    return OP_NO;
}

#undef DEF_CMD