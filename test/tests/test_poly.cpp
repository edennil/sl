#include <sstream>

#include <sl/core/macros.hpp>
#include <sl/core/to_value.hpp>
#include <sl/core/io_interface.hpp>
#include <sl/core/json_serializer.hpp>
#include <sl/core/helper.hpp>

#include <sl/core/json/helper.hpp>
#include "test_env.hpp"
#include "framework/helper.hpp"
#include "serialize_common.hpp"

namespace simple_inheritancy_test
{

    class base
    {
    public:

        ~base() = default;
        base() = default;

        virtual double& x() { return x_; }

        bool operator==(const base& ref) const
        {
            SL_TEST(x_ == ref.x_ );
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        double x_ = 0.;

        template<typename Ar>
        void serialize(Ar& ar, const unsigned int version) const
        {
            ar& SL_TO_SERIALIZE_LAST(SL_PROPERTY(base::x_, "x"));
        }

        template<typename Ar>
        void deserialize(Ar& ar, const unsigned int version)
        {
            ar& SL_TO_SERIALIZE_LAST(SL_PROPERTY(base::x_, "x"));
        }

    };

    class derived : public base
    {
    public:

        ~derived() = default;
        derived() = default;

        double& y() { return y_; }

        bool operator==(const derived& ref) const
        {
            static_cast<const base&>(*this) == static_cast<const base&>(ref);
            SL_TEST(y_ == ref.y_);
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        template<typename Ar>
        void serialize(Ar& ar, const unsigned int version) const
        {
            ar& SL_TMP_PROPERTIES(SL_CAST_BASE(base, *this), SL_GLOBAL_PROPERTY);
        }

        template<typename Ar>
        void deserialize(Ar& ar, const unsigned int version)
        {
            ar& SL_TMP_PROPERTIES(SL_CAST_BASE(base, *this), SL_GLOBAL_PROPERTY);
        }

        double y_ = 0.;

        SL_PROPERTIES(SL_PROPERTY(derived::y_, "y"));
    };
}

SL_TEST_GROUP(test_poly)

SL_ADD_TEST_CASE(simple_inheritancy)
{

    using namespace simple_inheritancy_test;

    derived d;
    d.x() = 1.;
    d.y() = 2.;

    auto is = std::is_polymorphic_v<base>;
    sl::test::serialize_common<derived, sl::json_iarchive, sl::json_oarchive>::apply(d);

}

SL_END_TEST_GROUP
