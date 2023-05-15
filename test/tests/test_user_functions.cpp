#include <vector>
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

SL_TEST_GROUP(test_user_functions)

class object_with_serialize
{
public:

    object_with_serialize() = default;
    ~object_with_serialize() = default;

    double& y() { return y_; }
    double y() const { return y_; }
    std::vector<double>& x() { return x_; }
    const std::vector<double>& x() const { return x_; }

    bool operator==(const object_with_serialize& ref) const
    {
        SL_TEST(x_ == ref.x_);
        SL_TEST(y_ == ref.y_);
        return true;
    }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void serialize(Ar& ar, const unsigned int version) const
    {
        ar& SL_TO_SERIALIZE(SL_PROPERTY(object_with_serialize::y_, "y")) & SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_with_serialize::x_, "x"));
    }

    double y_ = 0.;
    std::vector<double> x_;

    SL_PROPERTIES(SL_PROPERTY(object_with_serialize::y_, "y"), SL_PROPERTY(object_with_serialize::x_, "x"));

};

class object_with_deserialize
{
public:

    object_with_deserialize() = default;
    ~object_with_deserialize() = default;

    double& y() { return y_; }
    double y() const { return y_; }
    std::vector<double>& x() { return x_; }
    const std::vector<double>& x() const { return x_; }

    bool operator==(const object_with_deserialize& ref) const
    {
        SL_TEST(x_ == ref.x_);
        SL_TEST(y_ == ref.y_);
        return true;
    }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        ar& SL_TO_SERIALIZE(SL_PROPERTY(object_with_deserialize::y_, "y"))& SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_with_deserialize::x_, "x"));
    }

    double y_ = 0.;
    std::vector<double> x_;

    SL_PROPERTIES(SL_PROPERTY(object_with_deserialize::y_, "y"), SL_PROPERTY(object_with_deserialize::x_, "x"));

};

class object_with_serialize_deserialize
{
public:

    object_with_serialize_deserialize() = default;
    ~object_with_serialize_deserialize() = default;

    double& y() { return y_; }
    double y() const { return y_; }
    std::vector<double>& x() { return x_; }
    const std::vector<double>& x() const { return x_; }

    bool operator==(const object_with_serialize_deserialize& ref) const
    {
        SL_TEST(x_ == ref.x_);
        SL_TEST(y_ == ref.y_);
        return true;
    }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void serialize(Ar& ar, const unsigned int version) const
    {
        ar& SL_TO_SERIALIZE(SL_PROPERTY(object_with_serialize_deserialize::y_, "y"))& SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_with_serialize_deserialize::x_, "x"));
    }

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        ar& SL_TO_SERIALIZE(SL_PROPERTY(object_with_serialize_deserialize::y_, "y"))& SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_with_serialize_deserialize::x_, "x"));
    }

    double y_ = 0.;
    std::vector<double> x_;

};

class object_with_version
{
public:

    object_with_version() = default;
    ~object_with_version() = default;

    double& y() { return y_; }
    double y() const { return y_; }
    std::vector<double>& x() { return x_; }
    const std::vector<double>& x() const { return x_; }

    bool operator==(const object_with_version& ref) const
    {
        SL_TEST(x_ == ref.x_);
        SL_TEST(y_ == ref.y_);
        return true;
    }

protected:

    SL_SERIALIZABLE;

    double y_ = 0.;
    std::vector<double> x_;

    SL_PROPERTIES(SL_PROPERTY(object_with_version::y_, "y"), SL_PROPERTY(object_with_version::x_, "x"));

};

class object_deserialize_with_version
{
public:

    object_deserialize_with_version() = default;
    ~object_deserialize_with_version() = default;

    double& y() { return y_; }
    double y() const { return y_; }
    std::vector<double>& x() { return x_; }
    const std::vector<double>& x() const { return x_; }

    bool operator==(const object_deserialize_with_version& ref) const
    {
        SL_TEST(x_ == ref.x_);
        SL_TEST(y_ == ref.y_);
        return true;
    }

protected:

    SL_SERIALIZABLE;

    template<typename Ar>
    void deserialize(Ar& ar, const unsigned int version)
    {
        switch (version)
        {
            case 1:
            {
                ar& SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_deserialize_with_version::x_, "x"));
                break;
            }
            case 2:
            {
                ar& SL_TO_SERIALIZE(SL_PROPERTY(object_deserialize_with_version::y_, "y"))& SL_TO_SERIALIZE_LAST(SL_PROPERTY(object_deserialize_with_version::x_, "x"));
                break;
            }
        }
    }

    double y_ = 0.;
    std::vector<double> x_;

    SL_PROPERTIES(SL_PROPERTY(object_deserialize_with_version::y_, "y"), SL_PROPERTY(object_deserialize_with_version::x_, "x"));

};


SL_ADD_TEST_CASE(test_serialize_function)
{
    object_with_serialize a;
    a.x() = std::vector<double>{ 1., 2., 3. };
    a.y() = 2.;
    sl::test::serialize_common<object_with_serialize, sl::json_iarchive, sl::json_oarchive>::apply(a);
}

SL_ADD_TEST_CASE(test_deserialize_function)
{
    object_with_deserialize a;
    a.x() = std::vector<double>{ 1., 2., 3. };
    a.y() = 2.;
    sl::test::serialize_common<object_with_deserialize, sl::json_iarchive, sl::json_oarchive>::apply(a);
}

SL_ADD_TEST_CASE(test_serialize_and_deserialize_function)
{
    object_with_serialize_deserialize a;
    a.x() = std::vector<double>{ 1., 2., 3. };
    a.y() = 2.;
    sl::test::serialize_common<object_with_serialize_deserialize, sl::json_iarchive, sl::json_oarchive>::apply(a);
}

SL_ADD_TEST_CASE(test_with_version_function)
{
    object_with_version a;
    a.x() = std::vector<double>{ 1., 2., 3. };
    a.y() = 2.;
    sl::test::serialize_common<object_with_version, sl::json_iarchive, sl::json_oarchive>::apply(a);
}

SL_ADD_TEST_CASE(test_deserialize_with_version_function)
{
    object_deserialize_with_version a;
    a.x() = std::vector<double>{ 1., 2., 3. };
    a.y() = 2.;
    sl::test::serialize_common<object_deserialize_with_version, sl::json_iarchive, sl::json_oarchive>::apply(a);
}

SL_END_TEST_GROUP

SL_SERIALIZE_CLASS_NAME(test_user_functions::object_with_serialize, "object_with_serialize")
SL_SERIALIZE_CLASS_NAME(test_user_functions::object_with_deserialize, "object_with_deserialize")
SL_SERIALIZE_CLASS_NAME(test_user_functions::object_with_serialize_deserialize, "object_with_serialize_deserialize")
SL_SERIALIZE_CLASS_VERSION_AND_NAME(test_user_functions::object_with_version, 2, "object_with_version")
SL_SERIALIZE_CLASS_VERSION_AND_NAME(test_user_functions::object_deserialize_with_version, 2, "object_deserialize_with_version")