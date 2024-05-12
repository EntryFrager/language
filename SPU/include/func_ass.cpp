/// @file func_ass.cpp

#include "func_ass.h"

/**
 * Function that counts the number of commands.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int spu_ctor (SPU *spu, int argc, char **argv)
{
    my_assert (spu != NULL);

    if (argc == 2)
    {
        spu->file_name_input = (const char *) argv[1];
    }
    spu->file_name_print_bin = "../ass_output.bin";
    spu->file_name_print_txt = "../ass_output.log";

    spu->fp_print_bin = fopen (spu->file_name_print_bin, "w + b");
    spu->fp_print_txt = fopen (spu->file_name_print_txt, "w");

    if (spu->fp_print_bin == NULL || spu->fp_print_txt == NULL)
    {
        return ERR_FOPEN;
    }

    spu->label = (LABELS *) calloc (LABEL_CNT, sizeof (LABELS));
    my_assert (spu->label != NULL);

    return ERR_NO;
}

/**
 * Function that counts the number of commands.
 * @param[in] spu Structure containing all information
*/

void number_of_commands (SPU *spu)
{
    my_assert (spu != NULL);

    for (size_t ip = 0; ip <= spu->size_file; ip++)
    {
        if ((spu->buf_input[ip] == '\n' || spu->buf_input[ip] == '\0') && spu->buf_input[ip - 2] != '\n')
        {
            spu->n_cmd++;
            spu->n_words++;
        }
        else if (spu->buf_input[ip] == ' ' && spu->buf_input[ip - 1] != ' ' && spu->buf_input[ip + 1] != ' ')
        {
            spu->n_words++;
        }
    }
}

/**
 * Function returning file size.
 * @param[in] stream Pointer to file
 * @param[out] size_file File size
*/

size_t get_file_size (FILE *stream)
{
    my_assert (stream != NULL);

    size_t start = ftell (stream);
    fseek (stream, start, SEEK_END);
    size_t size_file = ftell (stream);
    rewind (stream);

    return size_file;
}

/**
 * Function that clears all variables.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int spu_dtor (SPU *spu)
{
    my_assert (spu != NULL)

    if (fclose (spu->fp_print_txt) != 0 || fclose (spu->fp_print_bin) != 0)
    {
        return ERR_FCLOSE;
    }

    free (spu->buf_input);
    free (spu->buf_output);
    free (spu->cmd);
    free (spu->label);

    spu->buf_input = NULL;
    spu->buf_output = NULL;
    spu->cmd = NULL;
    spu->label = NULL;

    spu->fp_input = NULL;
    spu->fp_print_txt = NULL;
    spu->fp_print_bin = NULL;

    spu->n_cmd = VALUE_DEFAULT;
    spu->size_file = VALUE_DEFAULT;

    return ERR_NO;
}
