#include <sstream>
#include "test_env.hpp"
#include "framework/helper.hpp"
#include <sl/core/macros.hpp>
#include <sl/core/to_value.hpp>
#include <sl/core/io_interface.hpp>
#include <sl/core/json_serializer.hpp>
#include <sl/core/helper.hpp>
#include <sl/core/json/helper.hpp>


class object_with_simple_elements
{
public:

    object_with_simple_elements() = default;
    ~object_with_simple_elements() = default;

    bool operator==(const object_with_simple_elements &ref)
    {
        SL_TEST(unsigned_int_number_ == ref.unsigned_int_number_);
        SL_TEST(long_number_ == ref.long_number_);
        SL_TEST(int_number_ == ref.int_number_);
        SL_TEST(double_number_ == ref.double_number_);
        SL_TEST(float_number_ == ref.float_number_);
        SL_TEST(size_t_number_ == ref.size_t_number_);
        SL_TEST(bool_value_ == ref.bool_value_);
        return true;
    }

    unsigned int& unsigned_int_number() { return unsigned_int_number_; }
    long& long_number() { return long_number_; }
    int& int_number() { return int_number_; }
    double& double_number() { return double_number_; };
    float& float_number() { return float_number_; }
    size_t& size_t_number() { return size_t_number_; }
    bool& bool_value() { return bool_value_; }

protected:

    SL_SERIALIZABLE;

    unsigned int unsigned_int_number_;
    long long_number_;
    int int_number_;
    double double_number_;
    float float_number_;
    size_t size_t_number_;
    bool bool_value_;

    SL_PROPERTIES(SL_PROPERTY(object_with_simple_elements::unsigned_int_number_, "unsigned_int_number"),
                  SL_PROPERTY(object_with_simple_elements::long_number_, "long_number"),
                  SL_PROPERTY(object_with_simple_elements::int_number_, "int_number"),
                  SL_PROPERTY(object_with_simple_elements::double_number_, "double_number"),
                  SL_PROPERTY(object_with_simple_elements::float_number_, "float_number"),
                  SL_PROPERTY(object_with_simple_elements::size_t_number_, "size_t_number"),
                  SL_PROPERTY(object_with_simple_elements::bool_value_, "bool_value"));
};

class object_with_std_elements
{
public:

    object_with_std_elements() = default;
    ~object_with_std_elements() = default;

    bool operator==(const object_with_std_elements &ref)
    {
        SL_TEST(std_string_ == ref.std_string_);
        SL_TEST(std_vector_ == ref.std_vector_);
        SL_TEST(std_map_ == ref.std_map_);
        SL_TEST(std_unordered_map_ == ref.std_unordered_map_);
        SL_TEST(std_unordered_multimap_ == ref.std_unordered_multimap_);
        SL_TEST(std_tuple_ == ref.std_tuple_);
        return true;
    }

    std::string& std_string() { return std_string_; }
    std::vector<double>& std_vector() { return std_vector_; }
    std::map<int, double>& std_map() { return std_map_; }
    std::unordered_map<std::string, double>& std_unordered_map() { return std_unordered_map_; }
    std::unordered_multimap<int, double>& std_unordered_multimap() { return std_unordered_multimap_; }
    std::tuple<std::string, double, int>& std_tuple() { return std_tuple_; }

protected:

    SL_SERIALIZABLE;

    std::string std_string_;
    std::vector<double> std_vector_;
    std::map<int, double> std_map_;
    std::unordered_map<std::string, double> std_unordered_map_;
    std::unordered_multimap<int, double> std_unordered_multimap_;
    std::tuple<std::string, double, int> std_tuple_;

    SL_PROPERTIES(  SL_PROPERTY(object_with_std_elements::std_string_, "std_string"),
                    SL_PROPERTY(object_with_std_elements::std_vector_, "std_vector"),
                    SL_PROPERTY(object_with_std_elements::std_map_, "std_map"),
                    SL_PROPERTY(object_with_std_elements::std_unordered_map_, "std_unordered_map"),
                    SL_PROPERTY(object_with_std_elements::std_unordered_multimap_, "std_unordered_multimap"),
                    SL_PROPERTY(object_with_std_elements::std_tuple_, "std_tuple"));

};

SL_TEST_GROUP(simple_tests)

template<typename T, typename IN, typename OUT>
struct test
{
    static void apply(const T &original)
    {
        T copy;
        std::stringstream stream;
        sl::save<std::stringstream, OUT, T>(stream, original);
        sl::load<std::stringstream, IN, T>(stream, copy);
        original == copy;
    }
};

SL_ADD_TEST_CASE(test_object_with_simple_elements)
{
    object_with_simple_elements ref;
    ref.unsigned_int_number() = 1;
    ref.long_number() = 2;
    ref.int_number() = 3;
    ref.double_number() = 4.;
    ref.float_number() = 5.f;
    ref.size_t_number() = 6;
    ref.bool_value() = true;
    test<object_with_simple_elements, sl::json_iarchive, sl::json_oarchive>::apply(ref);
}

SL_ADD_TEST_CASE(test_object_with_std_elements)
{
    object_with_std_elements ref;
    ref.std_string() = std::string("hi");
    ref.std_vector() = { 1., 2., 3. };
    {
        std::map<int, double> map;
        map.emplace(1, 1.);
        ref.std_map() = std::move(map);
    }
    {
        std::unordered_map<std::string, double> umap;
        umap.emplace("first", 1.);
        ref.std_unordered_map() = std::move(umap);
    }
    {
        std::unordered_multimap<int, double> ummap;
        ummap.emplace(1, 1);
        ///ummap.emplace(1, 2);
        //ummap.emplace(2, 1);
        ref.std_unordered_multimap() = std::move(ummap);
    }
    ref.std_tuple() = std::make_tuple(std::string("bye"), 3.1415, 1);
    test<object_with_std_elements, sl::json_iarchive, sl::json_oarchive>::apply(ref);
}

SL_END_TEST_GROUP

SL_SERIALIZE_CLASS(object_with_simple_elements)
SL_SERIALIZE_CLASS(object_with_std_elements)
