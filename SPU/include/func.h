/// @file func.h

#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "../include/error.h"                                                               ///< Connects a file that displays errors.
#include "../include/stack.h"                                                               ///< Connects the file that stores the stack

#define DEBUG                                                                               ///< Macro for program debugging.

#ifdef DEBUG
    #define CHECK_ERROR(code_error) if (code_error != ERR_NO) fprintf (stderr, "\x1b[31m%s\x1b[0m", my_strerr (code_error));
#else
    #define CHECK_ERROR(...)
#endif

enum COMMANDS_CODE {                                                                        ///< All command codes.
    HLT,                                                                                    ///< The hlt command that stops the program.
    OUT,                                                                                    ///< The command that prints the response.
    PUSH,                                                                                   ///< Command that adds an element to memory (stack).
    POP,                                                                                    ///< Command that removes an element from memory (stack).
    CALL,                                                                                   ///< Call command that preserves the previous position.
    RET,                                                                                    ///< Return command to label call command.
    JMP,                                                                                    ///< Unconditional jump command.
    JA,                                                                                     ///< Jump command if the last number written to the stack is greater than the second to last number written to the stack.
    JAE,                                                                                    ///< Jump command if the last number written to the stack is greater than or equal to the second to last number written to the stack.
    JB,                                                                                     ///< Jump command if the last number written to the stack is less than the second to last number written to the stack.
    JBE,                                                                                    ///< Jump command if the last number written to the stack is less than or equal to the second to last number written to the stack.
    JE,                                                                                     ///< Jump command if the last number written to the stack is equal to the second to last number written to the stack.
    JNE,                                                                                    ///< Jump command if the last number written to the stack is not equal to the second to last number written to the stack.
    JFR,                                                                                    ///< Jump team that only jumps on Friday.
    OUTC,                                                                                   ///< A command that prints characters to a file by their ASCII codes from the stack.
    ADD,                                                                                    ///< Addition command.
    SUB,                                                                                    ///< Subtraction command.
    MUL,                                                                                    ///< Multiply command.
    DIV,                                                                                    ///< Division command.
    SQRT,                                                                                   ///< Sine command.
    SIN,                                                                                    ///< Cosine command.
    COS,                                                                                    ///< Root command.
    IN,                                                                                     ///< A command that allows the user to enter a number using an input device.
    DRAW,                                                                                   ///< Command that starts graphics memory.
    INT,                                                                                    ///< A command that makes the last number pushed onto the stack an integer.
};

const size_t REG_CNT = 4;                                                                   ///< Number of registers.

const size_t LABEL_CNT = 20;                                                                ///< Maximum number of tags.

const size_t SIZE_RAM = 400;                                                                ///< RAM size.

const size_t ZERO_PTR = 0;                                                                  ///< Null pointer.

const int VALUE_DEFAULT = 0;                                                                ///< Default value of variables.

const size_t STACK_DEFAULT_SIZE = 10;                                                       ///< Default stack size.

const int HAVE_RAM = 1 << 7;                                                                ///< Code for a command interacting with RAM.
const int HAVE_REG = 1 << 6;                                                                ///< Code for a command that interacts with a register.
const int HAVE_ARG = 1 << 5;                                                                ///< Code for the command that interacts with the argument.

typedef struct {                                                                            ///< Structure containing all the information necessary for the program to work.
    const char *file_name_input = NULL;                                                     ///< File name to enter.
    const char *file_name_print = NULL;                                                     ///< File name for output.

    FILE *fp_input = NULL;                                                                  ///< Pointer to input file.
    FILE *fp_print = NULL;                                                                  ///< Pointer to output file.

    int *buf = NULL;                                                                        ///< A buffer containing all the information read from the file.

    size_t size_file = VALUE_DEFAULT;                                                       ///< File size.

    ELEMENT *reg_value = NULL;                                                              ///< Array containing register values.
    ELEMENT *ram_value = NULL;                                                              ///< RAM array.

    STACK stack      = {};                                                                  ///< Stack storing command arguments.
    STACK stack_call = {};                                                                  ///< Stack storing function call locations.
} SPU;

int input_text (SPU *spu);                                                                  ///< A function that reads text from a file into a buffer.

size_t get_file_size (FILE *stream);                                                        ///< Function returning file size.

#endif //FUNC_H
