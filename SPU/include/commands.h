DEF_CMD("hlt", HLT, false,
    {
        fprintf(spu->fp_print, "Расчёт окончен.");
        return;
    })

DEF_CMD("outc", OUTC, true,
    {
        print_text(spu, ip++, code_error);
        CHECK_BUF_IP(ip)
    })

DEF_CMD("out", OUTCOMM, false,
    {
        ELEMENT print_arg = POP(&spu->stack);
        fprintf(spu->fp_print, "Answer: %lg\n", print_arg);
    })

DEF_CMD("push", PUSH, true,
    {
        ELEMENT *arg_pointer = get_argument(spu, &ip, code_error);

        if (arg_pointer == NULL)
        {
            *code_error |= ERR_ARGC;
            return;
        }

        PUSH(&spu->stack, *arg_pointer);

        if ((spu->buf[ip] & HAVE_ARG) && !(spu->buf[ip] & HAVE_RAM) && !(spu->buf[ip] & HAVE_REG))
        {
            free(arg_pointer);
        }

        CHECK_BUF_IP(ip)
    })

DEF_CMD("pop", POP, true,
    {
        ELEMENT *arg_pointer = get_argument(spu, &ip, code_error);

        if (arg_pointer == NULL)
        {
            *code_error |= ERR_ARGC;
            return;
        }

        *arg_pointer = POP(&spu->stack);

        CHECK_BUF_IP(ip)
    })

DEF_CMD("add", ADD, false,
    {
        first_arg  = POP(&spu->stack);
        second_arg = POP(&spu->stack);

        PUSH(&spu->stack, first_arg + second_arg);
    })

DEF_CMD("sub", SUB, false,
    {
        first_arg  = POP(&spu->stack);
        second_arg = POP(&spu->stack);

        PUSH(&spu->stack, second_arg - first_arg);
    })

DEF_CMD("mul", MUL, false,
    {
        first_arg  = POP(&spu->stack);
        second_arg = POP(&spu->stack);

        PUSH(&spu->stack, first_arg * second_arg);
    })

DEF_CMD("div", DIV, false,
    {
        first_arg  = POP(&spu->stack);
        second_arg = POP(&spu->stack);

        PUSH(&spu->stack, second_arg / first_arg);
    })

DEF_CMD("sin", SIN, false,
    {
        PUSH(&spu->stack, (ELEMENT) sin(POP(&spu->stack)));
    })

DEF_CMD("cos", COS, false,
    {
        PUSH(&spu->stack, (ELEMENT) cos(POP(&spu->stack)));
    })

DEF_CMD("sqrt", SQRT, false,
    {
        PUSH(&spu->stack, (ELEMENT) sqrt(POP(&spu->stack)));
    })

DEF_CMD("pow", POW, false,
    {
        first_arg  = POP(&spu->stack);
        second_arg = POP(&spu->stack);

        PUSH(&spu->stack, (ELEMENT) pow(first_arg, second_arg));
    })

DEF_CMD("ln", LN, false,
    {
        PUSH(&spu->stack, (ELEMENT) log(POP(&spu->stack)));
    })

DEF_CMD("in", INCOMM, false,
    {
        ELEMENT value = 0;

        if (scanf("%lf", &value) != 1)
        {
            *code_error |= ERR_INPUT_ARG;
        }

        PUSH(&spu->stack, (ELEMENT) value);
    })

DEF_CMD("ret", RET, false,
    {
        ip = (size_t) POP(&spu->stack_call);
    })

DEF_CMD("floor", FLOOR, false,
    {
        int a = (int) POP(&spu->stack);
        PUSH(&spu->stack, (ELEMENT) a);
    })
