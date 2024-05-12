/// @file error.cpp

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"

/**
 * Types of errors.
*/

static const char* err_msgs_arr[] = {
    "ERROR NO.\n",
    "ERROR: an error occurred while opening the file.\n",
    "ERROR: an error occurred while closing the file.\n",
    "ERROR: an error occurred while reading text from the file.\n",
    "ERROR: invalid argument entered.\n",
    "ERROR: number of labels exceeded.\n",
    "ERROR: accessing RAM at the wrong address.\n",
    "ERROR: Invalid case entered.\n",
    "ERROR: invalid command entered.\n",
    "ERROR: incorrectly entered argument from stream input.\n",
    "ERROR: buffer out of bounds committed.\n",
};

/**
 * Error return function.
 * @param[in] code_error
*/

const char* my_strerr (unsigned code_error)
{
    if (code_error < ERROR_CNT)
    {
        return err_msgs_arr[code_error];
    }
    else
    {
        return "Unknown error.\n";
    }
}
