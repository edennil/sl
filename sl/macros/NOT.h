#pragma once
#include <macros/CHECK.h>

#define SL_NOT(x) SL_CHECK(SL_PRIMITIVE_CAT(SL_NOT_, x))
#define SL_NOT_0 0