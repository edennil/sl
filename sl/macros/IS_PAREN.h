#pragma once
#include <macros/CHECK.h>

#define SL_IS_PAREN(x) SL_CHECK(SL_IS_PAREN_PROBE x)
#define SL_IS_PAREN_PROBE(...) SL_PROBE(~)
#define SL_PROBE(x) x, 1
