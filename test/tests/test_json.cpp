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

    virtual double tmp() { return 0.; }

    double & a() { return a_; }
    double a() const { return a_; }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void serialize(Ar& ar, const unsigned int version) const
    {
        SL_TMP_PROPERTIES(SL_PROPERTY(A::a_, "value"));
        ar& std::make_tuple(tmp_properties, this);
    }

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        SL_TMP_PROPERTIES(SL_PROPERTY(A::a_, "value"));
        ar& std::make_tuple(tmp_properties, this);
    }

    double a_;

};

class B :public A
{
public:

    ~B() = default;
    B() = default;

    virtual double tmp() { return 1.; }
 
    double& b() { return b_; }
    double b() const { return b_; }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void serialize(Ar& ar, const unsigned int version) const
    {
        ar& std::make_tuple(SL_CAST_BASE(A, this), false) & std::make_tuple(properties, this);
    }

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        ar& std::make_tuple(SL_CAST_BASE(A, this), false) & std::make_tuple(properties, this);
    }

    double b_;

    SL_PROPERTIES(SL_PROPERTY(B::b_, "value"));
};

class C :public B
{
public:

    ~C() = default;
    C() = default;

    virtual double tmp() { return 2.; }

    double& c() { return c_; }
    double c() const { return c_; }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void serialize(Ar& ar, const unsigned int version) const
    {
        ar& std::make_tuple(SL_CAST_BASE(B, this), false) & std::make_tuple(properties, this);
    }

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        ar& std::make_tuple(SL_CAST_BASE(B, this), false) & std::make_tuple(properties, this);
    }

    double c_;

    SL_PROPERTIES(SL_PROPERTY(C::c_, "value"));
};


//void compare(const A &ref, const A &value)
//{
//    SL_TEST(ref.a() == value.a());
//}

SL_TEST_GROUP(json_test)

SL_ADD_TEST_CASE(simple_class_test)
{

    A* c = new C;

    c->a() = 1.;
    static_cast<B*>(c)->b() = 2.;
    static_cast<C*>(c)->c() = 3.;

    std::stringstream out;
    sl::save<std::stringstream, sl::json_oarchive, A *>(out, c);

    auto str = out.str();

    A* new_c = nullptr;
    sl::load<std::stringstream, sl::json_iarchive, A *>(out, new_c);
    
    //compare(a, new_a);

}

SL_END_TEST_GROUP

SL_SERIALIZE_CLASS(A)
SL_SERIALIZE_CLASS(B)
SL_SERIALIZE_CLASS(C)
SL_SERIALIZE_LINK_DERIVED(A, B)
SL_SERIALIZE_LINK_DERIVED(B, C)