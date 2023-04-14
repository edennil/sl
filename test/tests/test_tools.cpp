#include "test_env.hpp"
#include "framework/helper.hpp"

#include <sl/tools/multi_container.hpp>

void f(const std::vector<double>& l, const std::vector<double>& r)
{
    auto m = sl::tools::make_multi(l, r);

    for (auto v : m)
    {
        //std::get<0>(v) = 5;
        SL_TEST(std::get<0>(v) == std::get<1>(v));
    }
}

SL_TEST_GROUP(tools_test)

SL_ADD_TEST_CASE(test_multi_container_vector)
{
    std::vector<double> a{1.,2.,3.};
    std::vector<double> b{1.,2.2,3.};

    //auto m = sl::tools::make_multi(a, b);

    //for (auto v : m)
    //{
    //    std::get<0>(v) = 5;
    //    //SL_TEST(std::get<0>(v) == std::get<1>(v));
    //}

    f(a, b);
}

SL_END_TEST_GROUP