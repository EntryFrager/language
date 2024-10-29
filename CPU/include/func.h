#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "../../Include_file/error/error.h"
#include "../../Include_file/utilities/utils.h"
#include "./stack.h"

enum COMMANDS_CODE {
    HLT,
    OUTCOMM,
    PUSH,
    POP,
    CALL,
    RET,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,
    OUTC,
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,
    POW,
    LN,
    SIN,
    COS,
    INCOMM,
    FLOOR
};

const size_t REG_CNT = 4;
const size_t LABEL_CNT = 20;

const size_t SIZE_RAM = 10000;

const size_t ZERO_PTR = 0;

const int VALUE_DEFAULT = 0;
const size_t STACK_DEFAULT_SIZE = 10;

const int HAVE_RAM = 1 << 7;
const int HAVE_REG = 1 << 6;
const int HAVE_ARG = 1 << 5;

typedef struct {
    const char *file_name_input = NULL;
    const char *file_name_print = NULL;

    FILE *fp_input = NULL;
    FILE *fp_print = NULL;

    int64_t *buf = NULL;

    size_t size_file = VALUE_DEFAULT;

    ELEMENT *reg_value = NULL;
    ELEMENT *ram_value = NULL;

    STACK stack      = {};
    STACK stack_call = {};
} SPU;

int input_text (SPU *spu, int *code_error);

#endif //FUNC_H
