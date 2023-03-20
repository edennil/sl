#pragma once

#define SL_CAT(a, ...) SL_PRIMITIVE_CAT(a, __VA_ARGS__)
#define SL_PRIMITIVE_CAT(a, ...) a ##  __VA_ARGS__