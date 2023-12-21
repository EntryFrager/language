/// @file jump_cmd.h

#define jump_condition(expr)                                                                            \
    ip++;                                                                                               \
    CHECK_BUF_IP (ip)                                                                                   \
    first_arg = POP (&spu->stack);                                                                      \
    second_arg = POP (&spu->stack);                                                                     \
    if (first_arg expr second_arg)                                                                      \
    {                                                                                                   \
        ip = (size_t) spu->buf[ip] - 1;                                                                 \
    }                                                                                                   \

/**
 * Call command that preserves the previous position.
*/

DEF_CMD("call", CALL, true,
    {
        ip++;
        CHECK_BUF_IP (ip)
        PUSH (&spu->stack_call, ip);
        ip = (size_t) spu->buf[ip] - 1;
    })

/**
 * Unconditional jump command.
*/

DEF_CMD("jmp", JMP, true,
    {
        ip++;
        CHECK_BUF_IP (ip)
        ip = (size_t) spu->buf[ip] - 1;
    })

/**
 * Jump command if the last number written to the stack is greater than or equal to the second to last number written to the stack.
*/

DEF_CMD("jae", JAE, true,
    {
        jump_condition (>=);
    })

/**
 * Jump command if the last number written to the stack is less than or equal to the second to last number written to the stack.
*/

DEF_CMD("jbe", JBE, true,
    {
        jump_condition (<=);
    })

/**
 * Jump command if the last number written to the stack is greater than the second to last number written to the stack.
*/

DEF_CMD("ja", JA, true,
    {
        jump_condition (>);
    })

/**
 * Jump command if the last number written to the stack is less than the second to last number written to the stack.
*/

DEF_CMD("jb", JB, true,
    {
        jump_condition (<);
    })

/**
 * Jump command if the last number written to the stack is equal to the second to last number written to the stack.
*/

DEF_CMD("je", JE, true,
    {
        jump_condition (==);
    })

/**
 * Jump command if the last number written to the stack is not equal to the second to last number written to the stack.
*/

DEF_CMD("jne", JNE, true,
    {
        jump_condition (!=);
    })

#undef jump_condition