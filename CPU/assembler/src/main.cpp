#include "../inc/ass.h"

int main (int argc, char *argv[])
{
    int code_error = 0;

    Spu spu = {};


    printf("%d\n", sizeof("push"));

    spu_ctor(&spu, argc, argv, &code_error);

    input_text(&spu, &code_error);

    code_compilation(&spu, &code_error);

    spu_dtor(&spu, &code_error);

    PRINT_ERROR;

    return 0;
}
