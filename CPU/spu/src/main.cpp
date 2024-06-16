#include "../inc/spu.h"

int main ()
{
    int code_error = 0;

    SPU spu = {};

    spu_ctor(&spu, &code_error);

    input_text(&spu, &code_error);

    spu_run(&spu, &code_error);

    spu_dtor(&spu, &code_error);

    PRINT_ERROR;

    return 0;
}
