#ifndef CALC_H
#define CALC_H

#include "../../include/func.h"

const size_t WINDOWS_SIZE_X = 1000;
const size_t WINDOWS_SIZE_Y = 1000;

void spu_ctor (SPU *spu, int *code_error);

void spu_run (SPU *spu, int *code_error);

void spu_dtor (SPU *spu, int *code_error);

#endif
