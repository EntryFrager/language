#ifndef SPU_CPP
#define SPU_CPP

#include "../inc/spu.h"

#define PUSH(stack_name, arg) stack_push(stack_name, arg, code_error)

#define POP(stack_name) stack_pop(stack_name, code_error)

#define CHECK_RAM_IP(ptr) if (ptr > (SIZE_RAM - 1) || ptr < ZERO_PTR) {*code_error |= ERR_RAM;}

#define CHECK_BUF_IP(ptr) if (ptr > spu->size_file) {*code_error |= ERR_BUF_IP;}

static ELEMENT* get_argument (SPU *spu, size_t *ip, int *code_error);
static void print_text (SPU *spu, size_t ip, int *code_error);

void spu_ctor (SPU *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    stack_ctor(&spu->stack, STACK_DEFAULT_SIZE, code_error);
    stack_ctor(&spu->stack_call, LABEL_CNT, code_error);

    spu->file_name_input = "../../example_code/ass_output.bin";
    spu->file_name_print = "../../example_code/result.txt";

    fopen_init_(spu->fp_input, spu->file_name_input, "r + b");
    fopen_init_(spu->fp_print, spu->file_name_print, "w");

    calloc_init_(spu->ram_value, ELEMENT *, SIZE_RAM, sizeof(ELEMENT));
    calloc_init_(spu->reg_value, ELEMENT *, REG_CNT , sizeof(ELEMENT));
}

#define DEF_CMD(name, num, have_arg, code)              \
    case (num):                                         \
        {                                               \
            code                                        \
            break;                                      \
        }

void spu_run (SPU *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    fprintf(spu->fp_print, "This is a program that implements the functions of the processor.\n");

    for (size_t ip = 0; ip < spu->size_file; ip++)
    {
        ELEMENT first_arg  = VALUE_DEFAULT;
        ELEMENT second_arg = VALUE_DEFAULT;

        switch (spu->buf[ip] & 0x1F)
        {
            #include "../../include/commands.h"
            #include "../../include/jump_cmd.h"

            default:
                *code_error |= ERR_COMMAND;
        }

        CHECK_BUF_IP(ip);
    }
}

#undef DEF_CMD

ELEMENT* get_argument (SPU *spu, size_t *ip, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);
    my_assert(ip  != NULL, ERR_PTR);

    if ((spu->buf[*ip] & HAVE_RAM) && (spu->buf[*ip] & HAVE_REG) && (spu->buf[*ip] & HAVE_ARG))
    {
        if (spu->reg_value[spu->buf[(*ip) + 1] - 1] <= SIZE_RAM && spu->reg_value[spu->buf[(*ip) + 1] - 1] >= 0)
        {
            return &spu->ram_value[(size_t) spu->reg_value[spu->buf[++(*ip)] - 1] + spu->buf[++(*ip)]];
        }
    }
    else if ((spu->buf[*ip] & HAVE_RAM) && (spu->buf[*ip] & HAVE_REG))
    {
        if (spu->reg_value[spu->buf[(*ip) + 1] - 1] <= SIZE_RAM && spu->reg_value[spu->buf[(*ip) + 1] - 1] >= 0)
        {
            return &spu->ram_value[(size_t) spu->reg_value[spu->buf[++(*ip)] - 1]];
        }
    }
    else if (spu->buf[*ip] & HAVE_RAM)
    {
        return &spu->ram_value[spu->buf[++(*ip)]];
    }
    else if (spu->buf[*ip] & HAVE_REG)
    {
        return &spu->reg_value[spu->buf[++(*ip)] - 1];
    }
    else if (spu->buf[*ip] & HAVE_ARG)
    {
        calloc_(arg, ELEMENT *, 1, sizeof(ELEMENT));
        *arg = (ELEMENT) spu->buf[++(*ip)];
        return arg;
    }

    return NULL;
}

void print_text (SPU *spu, size_t ip, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    calloc_(outc, char *, spu->buf[++ip] + 1, sizeof(char));

    for (size_t i = 0; i < (size_t) spu->buf[ip]; i++)
    {
        *(outc + spu->buf[ip] - i - 1) = (char) ((int) POP(&spu->stack));
    }

    *(outc + spu->buf[ip]) = '\0';

    fprintf(spu->fp_print, "%s\n", outc);

    free(outc);
    outc = NULL;
}

void spu_dtor(SPU *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR)

    fclose_(spu->fp_input);
    fclose_(spu->fp_print);

    stack_dtor(&spu->stack, code_error);
    stack_dtor(&spu->stack_call, code_error);

    free(spu->buf);
    free(spu->reg_value);
    free(spu->ram_value);

    spu->buf       = NULL;
    spu->reg_value = NULL;
    spu->ram_value = NULL;

    spu->fp_input  = NULL;
    spu->fp_print  = NULL;

    spu->size_file = VALUE_DEFAULT;
}

#undef CHECK_BUF_IP
#undef CHECK_RAM_IP

#endif // SPU_CPP
