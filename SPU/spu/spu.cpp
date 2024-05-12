/// file @calc.cpp

#include "spu.h"

#define PUSH(stack_name, arg) stack_push (stack_name, arg)                                                              ///< Macro that adds an element to the stack.

#define POP(stack_name) stack_pop (stack_name)                                                                          ///< Macro that removes an element from the stack.

#define CHECK_RAM_IP(ptr) if (ptr > (SIZE_RAM - 1) || ptr < ZERO_PTR) return ERR_RAM;                                   ///< Checking that the ip of the RAM has not gone beyond the limits.

#define CHECK_BUF_IP(ptr) if (ptr > spu->size_file) return ERR_BUF_IP;                                ///< Checks that the buffer IP has not exceeded its limits.

/**
 * Function to initialize the spu structure.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int spu_ctor (SPU *spu)
{
    my_assert (spu != NULL);

    stack_ctor (&spu->stack, STACK_DEFAULT_SIZE);
    stack_ctor (&spu->stack_call, LABEL_CNT);

    spu->file_name_input = (const char *) "..\\ass_output.bin";
    spu->file_name_print = "..\\result.txt";

    spu->fp_input = fopen (spu->file_name_input, "r + b");
    spu->fp_print = fopen (spu->file_name_print, "wb");

    if (spu->fp_print == NULL || spu->fp_input == NULL)
    {
        return ERR_FOPEN;
    }

    spu->ram_value = (ELEMENT *) calloc (SIZE_RAM, sizeof (ELEMENT));
    my_assert (spu->ram_value != NULL);

    spu->reg_value = (ELEMENT *) calloc (REG_CNT , sizeof (ELEMENT));
    my_assert (spu->reg_value != NULL);

    return ERR_NO;
}

/**
 * Macro for code generation of commands
 * @param[in] name Сommand name
 * @param[in] num Command number
 * @param[in] have_arg The presence of argument
 * @param[in] code The code this command should execute
*/

#define DEF_CMD(name, num, have_arg, code)              \
    case (num):                                         \
        {                                               \
            code                                        \
            break;                                      \
        }
    
/**
 * A function that performs all calculations and outputs them to a file.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int spu_run (SPU *spu)
{
    my_assert (spu != NULL);

    fprintf (spu->fp_print, "This is a program that implements the functions of the processor.\n");

    for (size_t ip = 0; ip < spu->size_file; ip++)
    {
        ELEMENT first_arg  = VALUE_DEFAULT;
        ELEMENT second_arg = VALUE_DEFAULT;

        switch (spu->buf[ip] & 0x1F)
        {
            #include "..\include\commands.h"
            #include "..\include\jump_cmd.h"

            default:
                return ERR_COMMAND;
        }

        CHECK_BUF_IP (ip)
    }

    return ERR_NO;
}

#undef DEF_CMD

/**
 * A function that returns the address of a variable whose value is needed.
 * @param[in] spu Structure containing all information
 * @param[in] ip Command position in buffer
 * @param[out] argument element pointer
*/

ELEMENT* get_argument (SPU *spu, size_t ip)
{
    if ((spu->buf[ip] & HAVE_RAM) && (spu->buf[ip] & HAVE_REG))
    {
        return &spu->ram_value[(size_t) spu->reg_value[spu->buf[++ip] - 1]];
    }
    else if (spu->buf[ip] & HAVE_RAM)
    {
        return &spu->ram_value[spu->buf[++ip]];
    }
    else if (spu->buf[ip] & HAVE_REG)
    {
        return &spu->reg_value[spu->buf[++ip] - 1];
    }
    else if (spu->buf[ip] & HAVE_ARG)
    {
        ELEMENT a = (ELEMENT) spu->buf[++ip];
        return &a;
    }

    return NULL;
}

/**
 * Function that outputs text to a file
 * @param[in] spu Structure containing all information
 * @param[in] ip Command position in buffer
*/

void print_text (SPU *spu, size_t ip)
{
    char *outc = (char *) calloc (spu->buf[++ip] + 1, sizeof (char));
    my_assert (outc != NULL);

    for (size_t i = 0; i < (size_t) spu->buf[ip]; i++)
    {
        *(outc + spu->buf[ip] - i - 1) = (char) ((int) POP(&spu->stack));
    }

    *(outc + spu->buf[ip]) = '\0';
    
    fprintf (spu->fp_print, "%s\n", outc);

    free (outc);
    outc = NULL;
}

/**
 * Function that prints RAM values.
 * @param[in] ram Array with RAM values
*/

void graph_video (ELEMENT *ram)
{
    my_assert (ram != NULL);

    for (size_t ram_pos = 0; ram_pos < SIZE_RAM; ram_pos++)
    {
        if (ram_pos % 20 != 0)
        {
            if (ram[ram_pos] == 0)
            {
                printf ("\x1b[30m..\x1b[0m");
            }
            else
            {
                printf ("\x1b[31m**\x1b[0m");
            }
        }
        else
        {
            printf ("\n");
        }
    }
}

/**
 * Function that clears all variables.
 * @param[in] spu Structure containing all information
 * @param[out] code_error Returns the error code
*/

int spu_dtor(SPU *spu)
{
    my_assert (spu != NULL)

    if (fclose (spu->fp_print) != 0 || fclose (spu->fp_input) != 0)
    {
        return ERR_FCLOSE;
    }

    stack_dtor (&spu->stack);
    stack_dtor (&spu->stack_call);

    free (spu->buf);
    free (spu->reg_value);
    free (spu->ram_value);

    spu->buf       = NULL;
    spu->reg_value = NULL;
    spu->ram_value = NULL;

    spu->fp_input  = NULL;
    spu->fp_print  = NULL;

    spu->size_file = VALUE_DEFAULT;

    return ERR_NO;
}

#undef CHECK_BUF_IP
#undef CHECK_RAM_IP