
#include "test_env.hpp"
#include "framework/helper.hpp"

SL_TEST_GROUP(environment_test)

SL_ADD_TEST_CASE(operations)
{
    double a = 2.;
    double b = 2.;
    
    SL_TEST(a + b == 4.);
    SL_TEST(a - b == 0.);
    SL_TEST(a * b == 4.);
    SL_TEST(a / b == 1.);

    SL_TEST(a + b != 3.);
    SL_TEST(a + b > 3.);
    SL_TEST(a + b >= 4.);
    SL_TEST(a + b < 5.);
    SL_TEST(a + b <= 5.);
}

SL_ADD_TEST_CASE(simple_containers)
{
    {
        std::vector<double> a{ 1., 2., 3. };
        std::vector<double> b{ 1., 2., 3. };

        SL_TEST(a == b);
    }

    {
        std::set<double> a{ 1., 2., 3. };
        std::set<double> b{ 1., 2., 3. };

        SL_TEST(a == b);
    }

    {
        std::list<double> a{ 1., 2., 3. };
        std::list<double> b{ 1., 2., 3. };

        SL_TEST(a == b);
    }

    {
        std::map<std::string, int> a = { {"a", 1}, {"b", 2} };
        std::map<std::string, int> b = { {"b", 2}, {"a", 1} };

        SL_TEST(a == b);
    }

    {
        std::unordered_map<std::string, int> a = { {"a", 1}, {"b", 2} };
        std::unordered_map<std::string, int> b = { {"a", 1}, {"b", 2} };

        SL_TEST(a == b);
    }

    {
        std::multimap<std::string, int> a = { {"a", 1}, {"b", 2} };
        std::multimap<std::string, int> b = { {"a", 1}, {"b", 2} };

        SL_TEST(a == b);
    }

    {
        std::unordered_multimap<std::string, int> a = { {"a", 1}, {"b", 2} };
        std::unordered_multimap<std::string, int> b = { {"a", 1}, {"b", 2} };

        SL_TEST(a == b);
    }

}

SL_ADD_TEST_CASE(composed_containers)
{
    {
        std::vector<std::vector<double>> a{ {1., 2., 3.}, {4., 5., 6.} };
        std::vector<std::vector<double>> b{ {1., 2., 3.}, {4., 5., 6.} };

        SL_TEST(a == b);
    }

    {
        std::vector<std::multimap<int, double>> a{ {{1, 2.}, {2, 3.}}, {{2, 5.}, {1, 6.}} };
        std::vector<std::multimap<int, double>> b{ {{1, 2.}, {2, 3.}}, {{2, 5.}, {1, 6.}} };

        SL_TEST(a == b);
    }


    {
        std::multimap<std::string, std::multimap<int, double>> a{ {"a", { {1, 2.}, {2, 3.}}}, {"b", {{ {2, 5.}, {1, 6.}}}} };
        std::multimap<std::string, std::multimap<int, double>> b{ {"a", { {1, 2.}, {2, 3.}}}, {"b", {{ {2, 5.}, {1, 6.}}}} };

        SL_TEST(a == b);
    }
}


SL_END_TEST_GROUP