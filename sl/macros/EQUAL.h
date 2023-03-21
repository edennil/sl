#pragma once
#include <sl/macros/COMPL.h>
#include <sl/macros/CAT.h>
#include <sl/macros/IS_PAREN.h>
#include <sl/macros/EAT.h>
#include <sl/macros/BITAND.h>

// Problem with SL_IS_COMPARABLE

//#define SL_COMPARE_0(x) x
//#define SL_COMPARE_1(x) x
//#define SL_COMPARE_2(x) x
//#define SL_COMPARE_3(x) x
//#define SL_COMPARE_4(x) x
//#define SL_COMPARE_5(x) x
//
//#define SL_EQUAL(x, y) SL_COMP(SL_NO_EQUAL(x, y))
//
//#define SL_IS_COMPARABLE(x) SL_IS_PAREN(SL_CAT(SL_COMPARE_, x) (()) )
//
//#define SL_PRIMITIVE_COMPARE(x, y) SL_IS_PAREN \
//( \
//SL_COMPARE_ ## x ( SL_COMPARE_ ## y) (())  \
//)
//
//#define SL_NO_EQUAL(x, y) SL_NO_EQUAL_I(x, y)
//
//#define SL_NO_EQUAL_I(x, y) \
//SL_IIF(SL_BITAND(SL_IS_COMPARABLE(x))(SL_IS_COMPARABLE(y)) ) \
//( \
//   SL_PRIMITIVE_COMPARE, \
//   1 SL_EAT \
//)(x, y)

#define SL_EQUAL(x, y) SL_COMP(SL_NOT_EQUAL(x, y))

#define SL_NOT_EQUAL(x, y) SL_NOT_EQUAL_I(x, y)

#define SL_NOT_EQUAL_I(x, y) SL_CAT(SL_NOT_EQUAL_CHECK_, SL_NOT_EQUAL_ ## x(0, SL_NOT_EQUAL_ ## y))


#define SL_NOT_EQUAL_CHECK_SL_NIL 1

#define SL_NOT_EQUAL_CHECK_SL_NOT_EQUAL_1(c, y) 0
#define SL_NOT_EQUAL_CHECK_SL_NOT_EQUAL_2(c, y) 0
#define SL_NOT_EQUAL_CHECK_SL_NOT_EQUAL_3(c, y) 0
#define SL_NOT_EQUAL_CHECK_SL_NOT_EQUAL_4(c, y) 0

#define SL_NOT_EQUAL_1(c, y) SL_IIF(c)(SL_NIL, y(1, SL_NIL))
#define SL_NOT_EQUAL_2(c, y) SL_IIF(c)(SL_NIL, y(1, SL_NIL))
#define SL_NOT_EQUAL_3(c, y) SL_IIF(c)(SL_NIL, y(1, SL_NIL))
#define SL_NOT_EQUAL_4(c, y) SL_IIF(c)(SL_NIL, y(1, SL_NIL))
