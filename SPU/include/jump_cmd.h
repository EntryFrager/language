#define jump_condition(expr)                                                                            \
    ip++;                                                                                               \
    CHECK_BUF_IP(ip)                                                                                    \
    first_arg = POP(&spu->stack);                                                                       \
    second_arg = POP(&spu->stack);                                                                      \
    if (second_arg expr first_arg)                                                                      \
    {                                                                                                   \
        ip = (size_t) spu->buf[ip] - 1;                                                                 \
    }                                                                                                   \


DEF_CMD("call", CALL, true,
    {
        ip++;
        CHECK_BUF_IP(ip)
        PUSH(&spu->stack_call, ip);
        ip = (size_t) spu->buf[ip] - 1;
    })

DEF_CMD("jmp", JMP, true,
    {
        ip++;
        CHECK_BUF_IP(ip)
        ip = (size_t) spu->buf[ip] - 1;
    })

DEF_CMD("jae", JAE, true,
    {
        jump_condition(>=);
    })

DEF_CMD("jbe", JBE, true,
    {
        jump_condition(<=);
    })

DEF_CMD("ja", JA, true,
    {
        jump_condition(>);
    })

DEF_CMD("jb", JB, true,
    {
        jump_condition(<);
    })

DEF_CMD("je", JE, true,
    {
        jump_condition(==);
    })

DEF_CMD("jne", JNE, true,
    {
        jump_condition(!=);
    })

#undef jump_condition
