#ifndef LEXICAL_ANALYSIS_CPP
#define LEXICAL_ANALYSIS_CPP

#include "../inc/input.h"

static size_t    get_n_tokens         (char *str, size_t *n_ident, int *code_error);
static void      token_analysis       (Token *token, size_t n_token, TableName *table_name, char *str, int *code_error);
static int       table_name_add_ident (TableName *table_name, char *ident, int *cur_ident, types type, int *code_error);
static op_command is_key_word         (char **str, int *code_error);

void get_token (Tree *tree, int *code_error)
{
    my_assert(tree != NULL, ERR_PTR);

    char *str = tree->info.buf;

    tree->n_token = get_n_tokens(str, &tree->table_name.n_ident, code_error);
    ERR_RET();

    calloc_init_(tree->token, Token *, tree->n_token + 1, sizeof(Token));

    calloc_init_(tree->table_name.pairs_num_ident, PairsNumIdent *, tree->table_name.n_ident, sizeof(PairsNumIdent));

    token_analysis(tree->token, tree->n_token, &tree->table_name, str, code_error);
    ERR_RET();

    realloc_(tree->table_name.pairs_num_ident, PairsNumIdent *, tree->table_name.n_ident * sizeof(PairsNumIdent));
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

void token_analysis (Token *token, size_t n_token, TableName *table_name, char *str, int *code_error)
{
    my_assert(token      != NULL, ERR_PTR);
    my_assert(str        != NULL, ERR_PTR);
    my_assert(table_name != NULL, ERR_PTR);

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

            if (token[pos].type == OP && token[pos].data.types_op == FUNC)
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
            char *ident = read_ident(&str, code_error);
            ERR_RET();

            if (is_func)
            {
                token[pos].type = IDENT_FUNC;
                token[pos].data.ident = table_name_add_ident(table_name, ident, &cur_ident, IDENT_FUNC, code_error);
                ERR_RET();

                table_name->n_ident_func++;

                if (strcmp("main", ident) == 0)
                {
                    main_err += 1;
                }

                is_func = false;
            }
            else if (*str == '(')
            {
                token[pos].type = CALL_FUNC;
                token[pos].data.ident = table_name_add_ident(table_name, ident, &cur_ident, IDENT_FUNC, code_error);
                ERR_RET();

                my_assert(strcmp("main", ident) == 0, SYNTAX_ERROR)
            }
            else
            {
                token[pos].type = IDENT;
                token[pos].data.ident = table_name_add_ident(table_name, ident, &cur_ident, IDENT, code_error);
                ERR_RET();
            }
        }
    }

    my_assert(*str == '\0', SYNTAX_ERROR);

    my_assert(main_err == 1, SYNTAX_ERROR);

    token[n_token].type = DEF_TYPE;

    table_name->n_ident = cur_ident;
}

int table_name_add_ident (TableName *table_name, char *ident, int *cur_ident, types type, int *code_error)
{
    my_assert(table_name != NULL, ERR_PTR);
    my_assert(ident      != NULL, ERR_PTR);
    my_assert(cur_ident  != NULL, ERR_PTR);

    for (int i = 0; i < *cur_ident; i++)
    {
        if (strcmp(table_name->pairs_num_ident[i].ident, ident) == 0)
        {
            return i;
        }
    }

    table_name->pairs_num_ident[*cur_ident].type   = type;
    table_name->pairs_num_ident[*cur_ident].ident  = ident;
    table_name->pairs_num_ident[*cur_ident].number = *cur_ident;

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
