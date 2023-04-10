#include "test_env.hpp"
#include "framework/helper.hpp"

#include <sl/tools/multi_container.hpp>

SL_TEST_GROUP(tools_test)

SL_ADD_TEST_CASE(test_multi_container_vector)
{
    std::vector<double> a{1.,2.,3.};
    std::vector<double> b{1.,2.,3.};

    auto m = sl::tools::make_multi(a, b);

    for (auto &v : m)
    {
        SL_TEST(*std::get<0>(v) == *std::get<1>(v));
    }
}

SL_END_TEST_GROUP