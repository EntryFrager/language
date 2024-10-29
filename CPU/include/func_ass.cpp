#ifndef FUNC_ASS_CPP
#define FUNC_ASS_CPP

#include "func_ass.h"

void spu_ctor (Spu *spu, int argc, char **argv, int *code_error)
{
    my_assert (spu != NULL, ERR_PTR);

    if (argc == 2)
    {
        spu->file_name_input = (const char *) argv[1];
    }
    else
    {
        spu->file_name_input = "../../example_code/code.asm";
    }

    spu->file_name_print_bin = "../../example_code/ass_output.bin";
    spu->file_name_print_txt = "../../example_code/ass_output.log";

    fopen_init_(spu->fp_print_bin, spu->file_name_print_bin, "w + b");
    fopen_init_(spu->fp_print_txt, spu->file_name_print_txt, "w");

    calloc_init_(spu->label, Labels *, LABEL_CNT, sizeof(Labels));
}

void number_of_commands (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

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

void spu_dtor (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR)

    fclose_(spu->fp_print_txt);
    fclose_(spu->fp_print_bin);

    free(spu->buf_input);
    free(spu->buf_output);
    free(spu->cmd);
    free(spu->label);

    spu->buf_input = NULL;
    spu->buf_output = NULL;
    spu->cmd = NULL;
    spu->label = NULL;

    spu->fp_input = NULL;
    spu->fp_print_txt = NULL;
    spu->fp_print_bin = NULL;

    spu->n_cmd = VALUE_DEFAULT;
    spu->size_file = VALUE_DEFAULT;
}

#endif // FUNC_ASS_CPP
