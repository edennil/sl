#pragma once
#include <sl/macros/CAT.h>
#include <sl/macros/BOOL.h>


#define SL_IF(cond) SL_IIF(BOOL(cond))
#define SL_IIF(cond)  SL_PRIMITIVE_CAT(SL_IF_, cond)
#define SL_IF_0(t, ...) __VA_ARGS__
#define SL_IF_1(t, f) t