/// @file main_spu.cpp

#include "spu.h"

/**
 * A program that performs actions that have been previously converted by assembler into machine code.
*/

int main ()
{
    SPU spu = {};

    spu_ctor (&spu);

    int code_error = input_text (&spu);

    CHECK_ERROR (code_error)

    code_error = spu_run (&spu);

    CHECK_ERROR (code_error)

    code_error = spu_dtor (&spu);

    CHECK_ERROR (code_error)

    return 0;
}