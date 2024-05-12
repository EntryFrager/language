/// @file error.h

#ifndef ERROR_H
#define ERROR_H

/**
 * Macro checking the truth of an expression.
 * @param[in] expr Expression
*/

#define my_assert(expr) if (!(expr)) {  \
        fprintf(stderr, "%s %s:%d: My assertion failed: \"" #expr "\"\n", __FILE__, __func__, __LINE__); \
        abort (); \
    }

const int ERROR_CNT = 11;                       ///< Number of possible errors provided by the author.

enum code_error {                               ///< Error codes.
    ERR_NO,                                     ///< No error.
    ERR_FOPEN,                                  ///< Error opening file.
    ERR_FCLOSE,                                 ///< Error closing file.
    ERR_FREAD,                                  ///< Error when reading text from file.
    ERR_ARGC,                                   ///< Error reading argument.
    ERR_LABEL,                                  ///< Error reading label.
    ERR_RAM,                                    ///< Error when reading an argument associated with RAM.
    ERR_REG,                                    ///< Error reading register.
    ERR_COMMAND,                                ///< Error reading command.
    ERR_INPUT_ARG,                              ///< Invalid argument entered from stream input.
    ERR_BUF_IP,                                 ///< Buffer out of bounds committed.
};

const char* my_strerr (unsigned code_error);    ///< Error return function.

#endif // ERROR_H
