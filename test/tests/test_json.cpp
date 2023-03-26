#include <sstream>
#include "test_env.hpp"
#include "framework/helper.hpp"
#include <sl/core/macros.hpp>
#include <sl/core/to_value.hpp>
#include <sl/core/io_interface.hpp>
#include <sl/core/json_serializer.hpp>
#include <sl/core/helper.hpp>
#include <sl/core/json/helper.hpp>

class A
{
public:

    ~A() = default;
    A() = default;

    double & a() { return a_; }
    double a() const { return a_; }

protected:

    SL_SERIALIZABLE;

    double a_;

    SL_PROPERTIES(SL_PROPERTY(A::a_, "value"));

};

void compare(const A &ref, const A &value)
{
    SL_TEST(ref.a() == value.a());
}

SL_TEST_GROUP(json_test)

SL_ADD_TEST_CASE(simple_class_test)
{
    A a;
    a.a() = 5.;

    std::stringstream out;
    sl::save<std::stringstream, sl::json_oarchive, A>(out, a);

    A new_a;
    sl::load<std::stringstream, sl::json_iarchive, A>(out, new_a);
    
    compare(a, new_a);
}

SL_END_TEST_GROUP

SL_SERIALIZE_CLASS(A)