/// @file commands.h

/**
 * The hlt command that stops the program.
*/

DEF_CMD("hlt", HLT, false,
    {
        fprintf (spu->fp_print, "Расчёт окончен.");
        return ERR_NO;
    })

/**
 * This function takes as input the number of characters that need to be displayed on the screen. 
 * The ASCII codes of these characters must be added to the top of the stack in advance
*/

DEF_CMD("outc", OUTC, true,
    {
        print_text   (spu, ip++);
        CHECK_BUF_IP (ip)
    })

/**
 * The command that prints the response.
*/

DEF_CMD("out", OUT, false,
    {
        fprintf (spu->fp_print, "Answer: %lf\n", spu->stack.data[spu->stack.position - 1]);
    })

/**
 * Command that adds an element to memory (stack).
*/

DEF_CMD("push", PUSH, true,
    {
        ELEMENT *arg_pointer = get_argument (spu, ip++);
        PUSH (&spu->stack, *arg_pointer);

        CHECK_BUF_IP (ip)
    })

/**
 * Command that removes an element from memory (stack).
*/

DEF_CMD("pop", POP, true,
    {
        ELEMENT *arg_pointer = get_argument (spu, ip++);
        *arg_pointer = POP (&spu->stack);

        CHECK_BUF_IP (ip)
    })

/**
 * Addition command.
*/

DEF_CMD("add", ADD, false,
    {
        first_arg  = POP (&spu->stack);
        second_arg = POP (&spu->stack);

        PUSH (&spu->stack, first_arg + second_arg);
    })

/**
 * Subtraction command.
*/

DEF_CMD("sub", SUB, false,
    {
        first_arg  = POP (&spu->stack);
        second_arg = POP (&spu->stack);

        PUSH (&spu->stack, second_arg - first_arg);
    })

/**
 * Multiply command.
*/

DEF_CMD("mul", MUL, false,
    {
        first_arg  = POP (&spu->stack);
        second_arg = POP (&spu->stack);

        PUSH (&spu->stack, first_arg * second_arg);
    })

/**
 * Division command.
*/

DEF_CMD("div", DIV, false,
    {
        first_arg  = POP (&spu->stack);
        second_arg = POP (&spu->stack);

        PUSH (&spu->stack, second_arg / first_arg);
    })

/**
 * Sine command.
*/

DEF_CMD("sin", SIN, false,
    {
        PUSH (&spu->stack, (ELEMENT) sin (POP (&spu->stack)));
    })

/**
 * Cosine command.
*/

DEF_CMD("cos", COS, false,
    {
        PUSH (&spu->stack, (ELEMENT) cos (POP (&spu->stack)));
    })

/**
 * Root command.
*/

DEF_CMD("sqrt", SQRT, false,
    {
        PUSH (&spu->stack, (ELEMENT) sqrt (POP (&spu->stack)));
    })

/**
 * A command that makes the last number pushed on to the stack an integer.
*/

DEF_CMD("int", INT, false, 
    {
        int a = (int) POP (&spu->stack);
        PUSH (&spu->stack, (ELEMENT) a);
    })

/**
 * A command that allows the user to enter a number using an input device.
*/

DEF_CMD("in", IN, false,
    {
        ELEMENT value = 0;

        if (scanf ("%lf", &value) != 1)
        {
            return ERR_INPUT_ARG;
        }

        PUSH (&spu->stack, (ELEMENT) value);
    })

/**
 * Return command to label call command.
*/

DEF_CMD("ret", RET, false,
    {
        ip = (size_t) POP (&spu->stack_call);
    })

/**
 * Command that starts graphics memory.
*/

DEF_CMD("draw", DRAW, false,
    {
        graph_video (spu->ram_value);
    })