#ifndef FUNC_CPP
#define FUNC_CPP

#include "func.h"

int input_text (SPU* spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    spu->size_file = get_file_size(spu->fp_input, code_error) / sizeof(int64_t);

    calloc_init_(spu->buf, int64_t *, spu->size_file * sizeof(int64_t), sizeof(char));

    size_t read_size = fread(spu->buf, sizeof(int64_t), spu->size_file, spu->fp_input);
    my_assert(read_size == spu->size_file, ERR_FREAD);

    return ERR_NO;
}

#endif // FUNC_CPP
