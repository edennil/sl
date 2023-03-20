#include "test_env.hpp"
#include "framework/helper.hpp"

_SL_TEST_GROUP_(environment_test)

_SL_ADD_TEST_CASE_(operations)
{
    double a = 2.;
    double b = 2.;
    
    SL_TEST(a + b == 4);
    SL_TEST(a - b == 0);
    SL_TEST(a * b == 4);
    SL_TEST(a / b == 1);

    SL_TEST(a + b != 3);
    SL_TEST(a - b != 1);
    SL_TEST(a * b != 3);
    SL_TEST(a / b != 0);    
}


_SL_END_TEST_GROUP_