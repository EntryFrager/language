#ifndef STACK_CPP
#define STACK_CPP

#include "stack.h"

static const char *fp_err_name = "file_err.txt";

static const char *err_msgs_arr[] = {
    "NO ERROR.\n",
    "ERROR: null pointer to stack.\n",
    "ERROR: null pointer to stack data.\n",
    "ERROR: wrong stack size.\n",
    "ERROR: wrong stack position.\n",
    "ERROR: stack size is less than position.\n",
    "ERROR: error when opening file.\n",
    "ERROR: error when closing file.\n",
    "ERROR: your stack is full of shit, call ctor.\n",
    "ERROR: hash->struct does not match true value.\n",
    "ERROR: hash->data does not match true value.\n",
    "ERROR: error when checking stack canary.\n",
    "ERROR: error when checking stack left_canary.\n",
    "ERROR: error when checking stack right_canary.\n",
    "ERROR: error when checking stack data->canary.\n",
    "ERROR: error when checking stack data->left_canary.\n",
    "ERROR: error when checking stack data->right_canary.\n",
    "ERROR: error when allocating memory in stack_realloc.\n"
};

static const int UP = 1;
static const int DOWN = 2;

static const int HST_UP = 2;
static const int HST_DOWN = 4;

static int CODE_ERROR = 0;

static void stack_realloc(STACK *stk, const int size, int *code_error);

#ifdef HASH_CHECK
static HASH_TYPE hash_control_data (STACK *stk, int *code_error);

static HASH_TYPE hash_control_struct (STACK *stk, int *code_error);

static HASH_TYPE hash_djb (int str);
#endif

void stack_ctor (STACK *stk, const size_t capacity, int *code_error)
{
    my_assert(stk != NULL, ERR_PTR);
    my_assert(stk->data == NULL, ERR_PTR);

#ifdef CANARIES_CHECK
    calloc_init_(stk->data, ELEMENT *, capacity * sizeof(ELEMENT) + 2 * sizeof(CANARY_TYPE), sizeof(char));

    *((CANARY_TYPE *) stk->data) = CANARY;

    *((CANARY_TYPE *)(stk->data + capacity + sizeof(CANARY_TYPE) / sizeof(ELEMENT))) = CANARY;
    stk->data += sizeof(CANARY_TYPE) / sizeof(ELEMENT);

    stk->left_canary  = CANARY;
    stk->right_canary = CANARY;
#else
    calloc_init_(stk->data, ELEMENT *, capacity * sizeof(ELEMENT), sizeof(char));
#endif

    memset(stk->data, 0, capacity * sizeof(ELEMENT));

    stk->size = capacity;
    stk->position = 0;

    ON_DEBUG(stk->hash_data   = hash_control_data(stk),
             stk->hash_struct = hash_control_struct(stk));

    assert_stack(stk);
}

void stack_dtor(STACK *stk, int *code_error)
{
    my_assert(stk->data != NULL, ERR_PTR);
    assert_stack(stk);

#ifdef CANARIES_CHECK
    free(stk->data - sizeof(CANARY_TYPE) / sizeof(ELEMENT));
    stk->left_canary  = (CANARY_TYPE) STACK_VALUE_VENOM;
    stk->right_canary = (CANARY_TYPE) STACK_VALUE_VENOM;
#else
    free(stk->data);
#endif

    ON_DEBUG(stk->hash_data   = STACK_VALUE_VENOM,
             stk->hash_struct = STACK_VALUE_VENOM);

    stk->data = NULL;
    stk->size = STACK_VALUE_VENOM;
    stk->position = STACK_VALUE_VENOM;
}

void stack_push (STACK *stk, const ELEMENT value, int *code_error)
{
    assert_stack(stk);

    if (stk->size == stk->position)
    {
        stack_realloc(stk, UP, code_error);
    }

    stk->data[stk->position++] = value;

    ON_DEBUG(stk->hash_data   = hash_control_data(stk, code_error),
             stk->hash_struct = hash_control_struct(stk, code_error));

    assert_stack(stk);
}

ELEMENT stack_pop (STACK *stk, int *code_error)
{
    assert_stack(stk);

    if (stk->position < stk->size / HST_DOWN)
    {
        stack_realloc(stk, DOWN, code_error);
    }

    ELEMENT elem_pop = stk->data[--stk->position];

    stk->data[stk->position] = 0;

    ON_DEBUG(stk->hash_data   = hash_control_data(stk, code_error),
             stk->hash_struct = hash_control_struct(stk, code_error));

    assert_stack(stk);

    return elem_pop;
}

void stack_realloc (STACK *stk, const int type_mode, int *code_error)
{
    assert_stack(stk);

    int size = 0;

    if (type_mode == UP)
    {
        size = stk->size * HST_UP;
    }
    else if (type_mode == DOWN)
    {
        size = stk->size / HST_DOWN;
    }

#ifdef CANARIES_CHECK
    stk->data = (ELEMENT *) realloc(stk->data - sizeof(CANARY_TYPE) / sizeof(ELEMENT), size * sizeof(ELEMENT) + 2 * sizeof(CANARY_TYPE));
    my_assert(stk->data != NULL, ERR_MEM);

    *((CANARY_TYPE *)(stk->data + size + sizeof(CANARY_TYPE) / sizeof(ELEMENT))) = CANARY;

    stk->data += sizeof(CANARY_TYPE) / sizeof(ELEMENT);
#else
    realloc_(stk->data, ELEMENT *, size * sizeof(ELEMENT));
#endif

    if (size - stk->size > 0)
    {
        memset(stk->data + stk->size, 0, (size - stk->size) * sizeof(ELEMENT));
    }

    stk->size = size;

    ON_DEBUG(stk->hash_data   = hash_control_data(stk, code_error),
             stk->hash_struct = hash_control_struct(stk), code_error);

    assert_stack(stk);
}

#ifdef HASH_CHECK

HASH_TYPE hash_control_data (STACK *stk, int *code_error)
{
    my_assert(stk != NULL, ERR_PTR);

    HASH_TYPE hash_data = 0;

    for (int i = 0; i < stk->position; i++)
    {
        hash_data += hash_djb(stk->data[i]);
    }

    return hash_data;
}

HASH_TYPE hash_control_struct (STACK *stk, int *code_error)
{
    my_assert(stk != NULL, ERR_PTR);

    int data = 0;

    for (int i = 0; i < stk->position; i++)
    {
        data += stk->data[i];
    }

    HASH_TYPE hash = hash_djb(data + stk->position + stk->size);

    return hash;
}

HASH_TYPE hash_djb(int str)
{
    unsigned long hash = 5381;

    for (int i = 0; i < str; i++)
    {
        hash = ((hash << 5) + hash) + i;
    }

    return hash;
}

#endif

#ifdef DEBUG

int stack_verification (STACK *stk)
{
    if (stk == NULL)
    {
        return STACK_ERR;
    }

    if ( stk->position == STACK_VALUE_VENOM &&  stk->size == STACK_VALUE_VENOM)
    {
        return STACK_POINTER_GARBAGE;
    }

#ifdef HASH_CHECK
    if (stk->hash_struct != hash_control_struct (stk))
    {
        return STACK_HASH_STRUCT_ERR;
    }

    if (stk->hash_data != hash_control_data (stk))
    {
        return STACK_HASH_DATA_ERR;
    }
#endif

#ifdef CANARIES_CHECK
    if (stk->left_canary != CANARY && stk->right_canary != CANARY)
    {
        return STACK_CANARY_ERR;
    }

    if (stk->right_canary != CANARY)
    {
        return STACK_RIGHT_CANARY_ERR;
    }

    if (stk->left_canary != CANARY)
    {
        return STACK_LEFT_CANARY_ERR;
    }
#endif

    if (stk->data == NULL)
    {
        return STACK_DATA_ERR;
    }

    if (stk->position < 0)
    {
        return STACK_POSITION_ERR;
    }

    if (stk->size <= 0)
    {
        return STACK_SIZE_ERR;
    }

    if (stk->size < stk->position)
    {
        return STACK_SIZE_POS_ERR;
    }

#ifdef CANARIES_CHECK
    if (*(CANARY_TYPE *)(stk->data - sizeof (CANARY_TYPE) / sizeof (ELEMENT)) != CANARY && *(CANARY_TYPE *)(stk->data + stk->size) != CANARY)
    {
        return STACK_DATA_CANARY_ERR;
    }

    if (*(CANARY_TYPE *)(stk->data - sizeof (CANARY_TYPE) / sizeof (ELEMENT)) != CANARY)
    {
        return STACK_DATA_LEFT_CANARY_ERR;
    }

    if (*(CANARY_TYPE *)(stk->data + stk->size) != CANARY)
    {
        return STACK_DATA_RIGHT_CANARY_ERR;
    }
#endif


    return STACK_OK;
}

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err)
{
    FILE *fp_err = fopen (fp_err_name, "a");

    if (fp_err == NULL)
    {
        fprintf (stderr, "%s", err_msgs_arr[FILE_OPEN_ERR]);
    }

    if (stk != NULL)
    {
        if (code_error < STACK_ERROR_CNT)
        {
            fprintf (fp_err, "%s\n", err_msgs_arr[code_error]);
        }
        else
        {
            fprintf (fp_err, "Unknown error.\n\n");
        }

        fprintf (fp_err, "stack[%p] \"stk\" called from %s(%d) %s\n", stk, file_err, line_err, func_err);

        fprintf (fp_err, "{\n");

#ifdef CANARIES_CHECK
        fprintf (fp_err, "\tstack->left_canary = %llx\n", stk->left_canary);
#endif

        fprintf (fp_err, "\tposition = %d\n", stk->position);
        fprintf (fp_err, "\tsize = %d\n", stk->size);

        if (stk->data != NULL)
        {
            fprintf (fp_err, "\tdata[%p]\n", stk->data);
            fprintf (fp_err, "\t{\n");

#ifdef CANARIES_CHECK
            fprintf (fp_err, "\t\tdata->left_canary = %llx\n", *((CANARY_TYPE *)(stk->data - sizeof (CANARY_TYPE) / sizeof (ELEMENT))));
#endif

            if (stk->size > 0)
            {
                for (int i = 0; i < stk->size; i++)
                {
                    fprintf (fp_err, "\t\t*[%d] = %lf\n", i, stk->data[i]);
                }
            }
            else if (stk->position > 0)
            {
                for (int i = 0; i < stk->position; i++)
                {
                    fprintf (fp_err, "\t\t*[%d] = %lf\n", i, stk->data[i]);
                }
            }

#ifdef CANARIES_CHECK
            fprintf (fp_err, "\t\tdata->right_canary = %llx\n", *((CANARY_TYPE *) (stk->data + stk->size)));
#endif

            fprintf (fp_err, "\t}\n");
        }
        else
        {
            fprintf (fp_err, "\tdata[NULL]\n");
        }

        ON_DEBUG (fprintf (fp_err, "\tstack->hash_struct = %llu\n", stk->hash_struct),
                  fprintf (fp_err, "\tstack->hash_data = %llu\n", stk->hash_data));

#ifdef CANARIES_CHECK
        fprintf (fp_err, "\tstack->right_canary = %llx\n", stk->right_canary);
#endif

        fprintf (fp_err, "}\n\n-----------------------------------------------------------\n");

        if (fclose (fp_err) != 0)
        {
            fprintf (fp_err, "%s", err_msgs_arr[FILE_CLOSE_ERR]);
        }
    }
    else
    {
        if (code_error < STACK_ERROR_CNT)
        {
            fprintf (fp_err, "%s\n", err_msgs_arr[code_error]);
        }
        else
        {
            fprintf (fp_err, "Unknown error.\n\n");
        }

        fprintf (fp_err, "stack[NULL] \"stk\" called from %s(%d) %s\n", file_err, line_err, func_err);
    }
}

#endif

#endif // STACK_CPP
