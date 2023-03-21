#pragma once
#include <sl/macros/CAT.h>

#define SL_CHECK_N(x, n, ...) n
#define SL_CHECK(...) SL_CAT(SL_CHECK_N(__VA_ARGS__, 0, ),)
