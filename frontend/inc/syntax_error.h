#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#define DEF_SYNTAX_ERR(name) name,

static const char *syn_err_msgs_arr[] = {
    #include "./syntax_error_name.h"
};

#define syntax_assert(expr, cd_err) if (!(expr)) {  \
        fprintf(stderr, "\x1b[31m%s %s:%d: Syntax error: %s \x1b[0m\n", __FILE__, __func__, __LINE__, syn_err_msgs_arr[cd_err]); \
    }

enum syntax_error {
    NOT_MAIN_DEF        = 0,
    NOT_FUNC_DEF        = 1,
    NOT_IDENT_FUNC      = 2,
    NOT_OPEN_BRACKET    = 3,
    NOT_CLOSE_BRACKET   = 4,
    NOT_OPEN_BRACE      = 5,
    NOT_CLOSE_BRACE     = 6,
    NOT_END_EXPR        = 7,
    NOT_ASSIG           = 8,
    NOT_OP              = 9,

    UNKNOWN_ERROR  = 10
};

#endif // SYNTAX_ERROR_H
