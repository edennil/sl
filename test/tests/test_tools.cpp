#include "test_env.hpp"
#include "framework/helper.hpp"

#include <sl/tools/zip.hpp>

void f(const std::vector<double>& l, const std::vector<double>& r)
{
    for (auto v : sl::tools::zip(l, r))
    {
        SL_TEST(std::get<0>(v) == std::get<1>(v));
    }
}

SL_TEST_GROUP(tools_test)

SL_ADD_TEST_CASE(test_zip_vector)
{
    std::vector<double> a{1.,2.,3.};
    std::vector<double> b{1.,2.,3.};
    std::vector<double> c{ 1.,2.,3. };
    std::vector<double> d{ 1.,2.,3. };
    std::vector<double> e{ 1.,2.,3. };

    for (auto v : sl::tools::zip(a, b, c, d, e))
    {
        SL_TEST(std::get<0>(v) == std::get<4>(v));
    }

    f(a, b);
}

SL_ADD_TEST_CASE(test_zip_map)
{

    std::map<std::string, int> a = { {"a", 1}, {"b", 2}};
    std::map<std::string, int> b = { {"a", 1}, {"b", 2} };
    std::unordered_map<std::string, int> c = { {"a", 1}, {"b", 2} };
    std::unordered_map<std::string, int> d = { {"a", 1}, {"b", 2} };

    for (auto v : sl::tools::zip(a, b, c, d))
    {
        SL_TEST(std::get<0>(v).second == std::get<1>(v).second);
        SL_TEST(std::get<2>(v).second == std::get<3>(v).second);
    }

}

SL_ADD_TEST_CASE(test_zip_vector_map)
{

    std::vector<double> a{ 1.,2.,3. };
    std::vector<double> b{ 1.,2.,3. };
    std::unordered_map<std::string, int> c = { {"a", 1}, {"b", 2} };
    std::unordered_map<std::string, int> d = { {"a", 1}, {"b", 2} };

    for (auto v : sl::tools::zip(a, b, c, d))
    {
        SL_TEST(std::get<0>(v) == std::get<1>(v));
        SL_TEST(std::get<2>(v).second == std::get<3>(v).second);
    }

}


SL_END_TEST_GROUP