#pragma once
#include <macros/CAT.h>

#define SL_BITAND(y) SL_PRIMITIVE_CAT(SL_BITAND_, y)
#define SL_BITAND_0(y) 0
#define SL_BITAND_1(y) y
