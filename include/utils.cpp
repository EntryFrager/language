#include "utils.h"

char *get_file_to_str (FILE *stream, size_t *size_file, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);
    my_assert (size_file != NULL, ERR_PTR);

    *size_file = get_file_size (stream, code_error);
    ERR_RET (NULL);

    char *buf = (char *) calloc (*size_file + 1, sizeof (char));
    my_assert (buf != NULL, ERR_MEM);

    size_t read_size = fread (buf, sizeof (char), *size_file, stream);
    my_assert (read_size == *size_file, ERR_FREAD);

    buf[*size_file] = '\0';

    return buf;
}

size_t get_file_size (FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    size_t start = ftell (stream);
    fseek (stream, start, SEEK_END);
    size_t size_file = ftell (stream);
    rewind (stream);

    return size_file;
}

char *get_str (FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    char ch = 0;

    size_t len = 0;
    size_t str_len = 10;

    char *str = (char *) calloc (str_len, sizeof (char));
    my_assert (str != NULL, ERR_MEM);

    while (!isspace (ch = (char) fgetc (stream)))
    {
        str[len++] = ch;
        
        if (len == str_len)
        {
            str = (char *) realloc (str, str_len * 2);
            my_assert (str != NULL, ERR_MEM);

            str_len *= 2;
        }
    }

    str[len++] = '\0';
    str = (char *) realloc (str, len);
    my_assert (str != NULL, ERR_MEM);

    return str;
}

size_t get_n_lines (char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    size_t n_lines = 0;

    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
        {
            n_lines++;
        }
    }

    return n_lines;
}

char *skip_isspace (char *str, size_t len_str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    char *new_str = (char *) calloc (len_str, sizeof (char));
    my_assert (new_str != NULL, ERR_MEM);

    size_t pos = 0;

    for (size_t i = 0; i < len_str; i++)
    {
        if (!isspace (str[i]))
        {
            new_str[pos++] = str[i];
        }
    }

    new_str = (char *) realloc (new_str, (pos + 1) * sizeof (char));
    my_assert (new_str != NULL, ERR_MEM);

    new_str[pos] = '\0';

    free (str);

    return new_str;
}

char *read_ident (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    char *new_str = (char *) calloc (strlen (*str), sizeof (char));
    my_assert (new_str != NULL, ERR_MEM);
    size_t pos = 0;

    while (isalpha (**str) || **str == '_')
    {
        new_str[pos++] = **str;
        (*str)++;
    }

    new_str = (char *) realloc (new_str, pos + 1);
    my_assert (new_str != NULL, ERR_MEM);

    new_str[pos] = '\0';

    return new_str;
}

int compare_number (const double value_1, const double value_2, int *code_error)
{
    my_assert (isfinite (value_1), ERR_NAN);

    my_assert (isfinite (value_2), ERR_NAN);

    if ((value_1 - value_2) > EPSILON)
    {
        return 1;
    }
    else if ((value_1 - value_2) < (-1) * EPSILON)
    {
        return -1;
    }

    return 0;
}

void clean_buffer ()
{
    int ch = 0;

    while (((ch = getchar ()) != '\n') && (ch != EOF)) {}
}