#include "test_env.hpp"
#include "framework/helper.hpp"

SL_TEST_GROUP(environment_test)

SL_ADD_TEST_CASE(operations)
{
    double a = 2.;
    double b = 2.;
    
    SL_TEST(a + b == 4);
    SL_TEST(a - b == 0);
    SL_TEST(a * b == 4);
    SL_TEST(a / b == 1);

    SL_TEST(a + b != 3);
    SL_TEST(a + b > 3);
    SL_TEST(a + b >= 4);
    SL_TEST(a + b < 5);
    SL_TEST(a + b <= 5);
}


SL_END_TEST_GROUP