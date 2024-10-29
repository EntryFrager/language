#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../Include_file/error/error.h"
#include "../../Include_file/utilities/utils.h"

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

enum REG {
    RAX = 1,
    RBX,
    RCX,
    RDX
};

const size_t LABEL_CNT = 40;

const int HAVE_RAM = 1 << 7;
const int HAVE_REG = 1 << 6;
const int HAVE_ARG = 1 << 5;

const int YES_ARGUMENT = 1;
const int NO_ARGUMENT  = -1;

const int VALUE_DEFAULT = -1;

typedef struct {
    int label_ip = VALUE_DEFAULT;
    char *name_label = NULL;
    int size_label = VALUE_DEFAULT;
} Labels;

typedef struct {
    size_t size_str = VALUE_DEFAULT;
    char *command = NULL;

    int cmd_code = VALUE_DEFAULT;
    double argc = VALUE_DEFAULT;
    int reg = VALUE_DEFAULT;
} Commands;

typedef struct {
    const char *file_name_input = NULL;
    const char *file_name_print_txt = NULL;
    const char *file_name_print_bin = NULL;

    FILE *fp_input = NULL;
    FILE *fp_print_txt = NULL;
    FILE *fp_print_bin = NULL;

    char *buf_input = NULL;
    double *buf_output = NULL;

    size_t size_file = 0;
    size_t n_cmd = 0;
    size_t n_words = 0;
    size_t n_label = 0;

    Commands *cmd = NULL;
    Labels *label = NULL;
} Spu;

void spu_ctor (Spu *spu, int argc, char **argv, int *code_error);

void number_of_commands (Spu *spu, int *code_error);

void spu_dtor (Spu *spu, int *code_error);

#endif //FUNC_H
