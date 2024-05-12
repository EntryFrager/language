/// @file func.cpp

#include "func.h"

/**
 * Function that reads information from a file.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int input_text (SPU* spu)
{
    my_assert (spu != NULL);

    spu->size_file = get_file_size (spu->fp_input) / 4;

    spu->buf = (int *) calloc (spu->size_file * sizeof (int), sizeof (char));
    my_assert (spu->buf != NULL);

    size_t read_size = fread (spu->buf, sizeof (int), spu->size_file, spu->fp_input);

    if (read_size != spu->size_file)
    {
        return ERR_FREAD;
    }

    return ERR_NO;
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