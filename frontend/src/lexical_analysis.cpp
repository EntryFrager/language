#ifndef LEXICAL_ANALYSIS_CPP
#define LEXICAL_ANALYSIS_CPP

#include "../inc/input.h"

static size_t     get_n_tokens         (char *str, size_t *n_ident, int *code_error);
static void       token_analysis       (Token *token, size_t n_token, Identificators *idents, char *str, int *code_error);
static int        table_name_add_ident (Identificators *idents, char *name_var, int *cur_ident, int *code_error);
static op_command is_key_word          (char **str, int *code_error);

void get_token (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    char *str = tree->info.buf;

    tree->n_token = get_n_tokens(str, &tree->idents.n_ident, code_error);
    ERR_RET();

    calloc_init_(tree->token, Token *, tree->n_token + 1, sizeof(Token));

    calloc_init_(tree->idents.ident, Ident *, tree->idents.n_ident, sizeof(Ident));

    token_analysis(tree->token, tree->n_token, &tree->idents, str, code_error);
    ERR_RET();
}

size_t get_n_tokens (char *str, size_t *n_ident, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    size_t n_tokens = 0;

    while (*str != '\0')
    {
        if (is_key_word(&str, code_error) != OP_NO)
        {
            n_tokens++;
        }
        else if (isdigit(*str) || isalpha(*str) || *str == '-' || *str == '_')
        {
            if (isalpha(*str) || *str == '_')
            {
                (*n_ident)++;
            }

            n_tokens++;

            while (isdigit(*str) || isalpha(*str) || *str == '_')
            {
                str++;
            }
        }
    }

    return n_tokens;
}

void token_analysis (Token *token, size_t n_token, Identificators *idents, char *str, int *code_error)
{
    my_assert(idents != NULL, ERR_PTR);
    my_assert(token  != NULL, ERR_PTR);
    my_assert(str    != NULL, ERR_PTR);

    op_command op = OP_NO;

    int cur_ident = 0;

    bool is_func = false;

    int main_err = 0;

    for (size_t pos = 0; pos < n_token; pos++)
    {
        if ((op = is_key_word(&str, code_error)) != OP_NO)
        {
            token[pos].type = OP;
            token[pos].data.types_op = op;

            if (op == FUNC)
            {
                is_func = true;
            }
        }
        else if (isdigit(*str) || *str == '-')
        {
            token[pos].type = NUM;

            int n_read = 0;
            sscanf(str, "%lf%n", &token[pos].data.value, &n_read);

            str += n_read;
        }
        else if (isalpha(*str) || *str == '_')
        {
            char *name_var = read_ident(&str, code_error);
            ERR_RET();

            if (is_func)
            {
                token[pos].type = IDENT_FUNC;
                token[pos].data.ident = table_name_add_ident(idents, name_var, &cur_ident, code_error);
                ERR_RET();

                idents->n_funcs++;

                if (strcmp("main", name_var) == 0)
                {
                    main_err += 1;
                }

                is_func = false;
            }
            else if (*str == '(')
            {
                token[pos].type = CALL_FUNC;
                token[pos].data.ident = table_name_add_ident(idents, name_var, &cur_ident, code_error);
                ERR_RET();
            }
            else
            {
                token[pos].type = IDENT;
                token[pos].data.ident = table_name_add_ident(idents, name_var, &cur_ident, code_error);
                ERR_RET();
            }
        }
    }

    my_assert(*str == '\0', SYNTAX_ERROR);

    my_assert(main_err == 1, SYNTAX_ERROR);

    token[n_token].type = DEF_TYPE;

    idents->n_ident = cur_ident;
}

int table_name_add_ident (Identificators *idents, char *name_var, int *cur_ident, int *code_error)
{
    my_assert(idents    != NULL, ERR_PTR);
    my_assert(name_var  != NULL, ERR_PTR);
    my_assert(cur_ident != NULL, ERR_PTR);

    for (int i = 0; i < *cur_ident; i++)
    {
        if (strcmp(idents->ident[i].name_var, name_var) == 0)
        {
            return i;
        }
    }

    idents->ident[*cur_ident].name_var = name_var;
    idents->ident[*cur_ident].n_var    = *cur_ident;

    (*cur_ident)++;

    return (*cur_ident) - 1;
}

#define DEF_CMD(func_code, name)                  \
    if (strncmp(*str, name, str_len(name)) == 0)  \
    {                                             \
        *str += str_len(name);                    \
        return func_code;                         \
    } else

op_command is_key_word (char **str, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    #include "../../tree/comand.h"
    {}

    return OP_NO;
}

#undef DEF_CMD

#endif // LEXICAL_ANALYSIS_CPP
