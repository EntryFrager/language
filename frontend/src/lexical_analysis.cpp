#ifndef LEXICAL_ANALYSIS_CPP
#define LEXICAL_ANALYSIS_CPP

#include "../inc/input.h"
#include "../inc/syntax_error.h"

static size_t     get_n_tokens         (char *str, size_t *n_ident, size_t *n_funcs, int *code_error);
static void       token_analysis       (Token *token, size_t n_token, Identificators *idents, char *str, int *code_error);
static int        add_name_ident       (Identificators *idents, char *name_var, int n_scope, int *cur_ident, int *code_error);
static op_command token_is_key_word    (char **str, int *code_error);
static bool       token_is_number      (char *str, int *code_error);
static bool       token_is_ident       (char *str, int *code_error);

void get_token (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    char *str = tree->info.buf;

    tree->n_token = get_n_tokens(str, &tree->idents.n_ident, &tree->idents.n_funcs, code_error);
    ERR_RET();

    calloc_init_(tree->token, Token *, tree->n_token + 1, sizeof(Token));

    calloc_init_(tree->idents.ident, Ident *, tree->idents.n_ident, sizeof(Ident));

    token_analysis(tree->token, tree->n_token, &tree->idents, str, code_error);
    ERR_RET();
}

size_t get_n_tokens (char *str, size_t *n_ident, size_t *n_funcs, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    size_t n_tokens = 0;

    op_command op = OP_NO;

    while (*str != '\0')
    {
        if ((op = token_is_key_word(&str, code_error)) != OP_NO)
        {
            n_tokens++;

            if (op == FUNC)
            {
                (*n_funcs)++;
            }
        }
        else if (token_is_number(str, code_error))
        {
            str++;

            while (isdigit(*str))
            {
                str++;
            }

            n_tokens++;
        }
        else if (isalpha(*str) || *str == '_')
        {
            str++;

            while (isalpha(*str) || *str == '_' || isdigit(*str))
            {
                str++;
            }

            (*n_ident)++;
            n_tokens++;
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

    int ident_code = 0;
    int ident_func_code = 0;
    int n_scope = -1;

    bool is_func = false;

    int main_err = 0;

    for (size_t pos = 0; pos < n_token; pos++)
    {
        if ((op = token_is_key_word(&str, code_error)) != OP_NO)
        {
            token[pos].type = OP;
            token[pos].data.types_op = op;

            if (op == FUNC)
            {
                is_func = true;
                ident_code = 0;
                n_scope++;
            }
        }
        else if (token_is_number(str, code_error))
        {
            token[pos].type = NUM;

            int n_read = 0;
            sscanf(str, "%lf%n", &token[pos].data.value, &n_read);

            str += n_read;
        }
        else if (token_is_ident(str, code_error))
        {
            char *name_var = read_ident(&str, code_error);
            ERR_RET();

            if (is_func)
            {
                token[pos].type = IDENT_FUNC;
                token[pos].data.ident = add_name_ident(idents, name_var, -1, &ident_func_code, code_error);
                ERR_RET();

                if (strcmp("main", name_var) == 0)
                {
                    main_err += 1;
                }

                is_func = false;
            }
            else if (*str == '(')
            {
                token[pos].type = CALL_FUNC;
                token[pos].data.ident = add_name_ident(idents, name_var, -1, &ident_func_code, code_error);
                ERR_RET();
            }
            else
            {
                token[pos].type = IDENT;
                token[pos].data.ident = add_name_ident(idents, name_var, n_scope, &ident_code, code_error);
                ERR_RET();
            }
        }
    }

    my_assert(*str == '\0', SYNTAX_ERROR);

    syntax_assert(main_err == 1, NOT_MAIN_DEF);

    token[n_token].type = DEF_TYPE;

    idents->n_ident = idents->cur_ident;
}

int add_name_ident (Identificators *idents, char *name_var, int n_scope, int *ident_code, int *code_error)
{
    my_assert(idents     != NULL, ERR_PTR);
    my_assert(name_var   != NULL, ERR_PTR);
    my_assert(ident_code != NULL, ERR_PTR);

    for (size_t i = 0; i < idents->cur_ident; i++)
    {
        if (strcmp(idents->ident[i].name_var, name_var) == 0 && idents->ident[i].n_scope == n_scope)
        {
            return idents->ident[i].n_var;
        }
    }

    idents->ident[idents->cur_ident].name_var = name_var;
    idents->ident[idents->cur_ident].n_var    = *ident_code;
    idents->ident[idents->cur_ident].n_scope  = n_scope;
    idents->cur_ident++;
    (*ident_code)++;

    return (*ident_code) - 1;
}

#define DEF_CMD(func_code, name)                  \
    if (strncmp(*str, name, str_len(name)) == 0)  \
    {                                             \
        *str += str_len(name);                    \
        return func_code;                         \
    } else

op_command token_is_key_word (char **str, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    #include "../../tree/comand.h"
    {}

    return OP_NO;
}

bool token_is_number (char *str, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    return (bool) (isdigit(*str) || *str == '-');
}

bool token_is_ident (char *str, int *code_error)
{
    my_assert(str != NULL, ERR_PTR);

    return (bool) (isalpha(*str) || *str == '_');
}

#endif // LEXICAL_ANALYSIS_CPP
