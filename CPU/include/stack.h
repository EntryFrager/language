/// @file stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../../Include_file/error/error.h"
#include "../../Include_file/utilities/utils.h"

#ifdef DEBUG
    //#define CANARIES_CHECK
    //#define HASH_CHECK
#endif

#ifdef HASH_CHECK
    #define ON_DEBUG(...)                                               \
    __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#ifdef DEBUG
    #define assert_stack(stack) {                                           \
        if ((CODE_ERROR = stack_verification (stack)) != STACK_OK)          \
        {                                                                   \
            stack_dump (stack, CODE_ERROR, __FILE__, __func__, __LINE__);   \
            abort ();                                                       \
        }                                                                   \
    }
#else
    #define assert_stack(...)
#endif

enum stack_code_error
{
    STACK_OK,
    STACK_ERR,
    STACK_DATA_ERR,
    STACK_SIZE_ERR,
    STACK_POSITION_ERR,
    STACK_SIZE_POS_ERR,
    FILE_OPEN_ERR,
    FILE_CLOSE_ERR,
    STACK_POINTER_GARBAGE,

    STACK_HASH_STRUCT_ERR,
    STACK_HASH_DATA_ERR,
    STACK_CANARY_ERR,
    STACK_LEFT_CANARY_ERR,
    STACK_RIGHT_CANARY_ERR,
    STACK_DATA_CANARY_ERR,
    STACK_DATA_LEFT_CANARY_ERR,
    STACK_DATA_RIGHT_CANARY_ERR,
    ERR_STACK_REALLOC,
};

typedef double ELEMENT;

const int STACK_VALUE_VENOM = -100000;

#ifdef CANARIES_CHECK
    typedef unsigned long long CANARY_TYPE;

    const CANARY_TYPE CANARY = 0xDEDEDEDEDEDEDEDE;
#endif

#ifdef HASH_CHECK
    typedef unsigned long long HASH_TYPE;
#endif

typedef struct {
#ifdef CANARIES_CHECK
    CANARY_TYPE left_canary = CANARY;
#endif

    ELEMENT *data = NULL;

    int size = STACK_VALUE_VENOM;
    int position = STACK_VALUE_VENOM;

#ifdef HASH_CHECK
    HASH_TYPE hash_data = (HASH_TYPE) STACK_VALUE_VENOM;
    HASH_TYPE hash_struct = (HASH_TYPE) STACK_VALUE_VENOM;
#endif

#ifdef CANARIES_CHECK
    CANARY_TYPE right_canary = CANARY;
#endif
} STACK;

const int STACK_ERROR_CNT = 17;

void stack_ctor (STACK *stk, const size_t capacity, int *code_error);

void stack_dtor (STACK *stk, int *code_error);

void stack_push (STACK *stk, const ELEMENT value, int *code_error);

ELEMENT stack_pop (STACK *stk, int *code_error);

int stack_verification (STACK *stk);

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err);

#endif // STACK_H
