#include <sstream>
#include <memory>

#include <sl/core/macros.hpp>
#include <sl/core/to_value.hpp>
#include <sl/core/io_interface.hpp>
#include <sl/core/json_serializer.hpp>
#include <sl/core/helper.hpp>
#include <sl/core/json/helper.hpp>

#include "test_env.hpp"
#include "framework/helper.hpp"
#include "serialize_common.hpp"

namespace test_simple_double_ptr
{

    class observer
    {
    public:

        ~observer() = default;
        observer() = default;

        void link(double* ptr) { ptr_ = ptr; }

        bool operator==(const observer& ref) const
        {
            SL_TEST(*ptr_ == *ref.ptr_);
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        double* ptr_ = nullptr;

        SL_PROPERTIES(SL_PROPERTY(observer::ptr_, "pointer"));

    };

    template<typename T>
    class object
    {
    public:

        ~object() 
        {
            if constexpr (std::is_pointer_v<T>)
            {
                delete ptr_;
            }
        }

        object() = default;

        object(object&& src) noexcept
        {
            std::swap(ptr_, src.ptr_);
        }

        T & ptr() { return ptr_; }

        bool operator==(const object& ref) const
        {
            SL_TEST(*ptr_ == *ref.ptr_);
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        T ptr_ = nullptr;

        SL_PROPERTIES(SL_PROPERTY(object::ptr_, "pointer"));

    };

} // namespace test_simple_double_ptr

namespace test_simple_double_ptr_array
{

    class observer
    {
    public:

        ~observer() = default;
        observer() = default;

        void link(std::size_t size, double* ptr) { size_ = size; ptr_ = ptr; }

        bool operator==(const observer& ref) const
        {
            SL_TEST(size_ == ref.size_);
            for (std::size_t i = 0; i < size_; i++)
            {
                SL_TEST(ptr_[i] == ref.ptr_[i]);
            }
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        std::size_t size_ = 0;
        double* ptr_ = nullptr;

        SL_PROPERTIES(SL_ARRAY_PROPERTY(observer::size_, observer::ptr_, "pointer"));

    };

    class object
    {
    public:

        ~object() { delete [] ptr_; }
        object() = default;

        object(object&& src) noexcept
        {
            std::swap(size_, src.size_);
            std::swap(ptr_, src.ptr_);
        }

        double *&ptr() { return ptr_; }
        std::size_t& size() { return size_; }

        bool operator==(const object& ref) const
        {
            SL_TEST(size_ == ref.size_);
            for (std::size_t i = 0; i < size_; i++)
            {
                SL_TEST(ptr_[i] == ref.ptr_[i]);
            }
            return true;
        }

    protected:

        SL_SERIALIZABLE;

        std::size_t size_ = 0;
        double* ptr_ = nullptr;

        SL_PROPERTIES(SL_ARRAY_PROPERTY(object::size_, object::ptr_, "pointer"));

    };

} // namespace test_simple_double_ptr_array


SL_TEST_GROUP(test_ptr)

SL_ADD_TEST_CASE(simple_double_ptr)
{

    {
        test_simple_double_ptr::object<double*> obj;
        obj.ptr() = new double{ 3. };
        sl::test::serialize_common<test_simple_double_ptr::object<double *>, sl::json_iarchive, sl::json_oarchive>::apply(obj);
    }

    {
        test_simple_double_ptr::object<std::unique_ptr<double>> obj;
        obj.ptr() = std::make_unique<double>(3.);
        sl::test::serialize_common<test_simple_double_ptr::object<std::unique_ptr<double>>, sl::json_iarchive, sl::json_oarchive>::apply(obj);
    }

    {
        test_simple_double_ptr::object<std::shared_ptr<double>> obj;
        obj.ptr() = std::make_shared<double>(3.);
        sl::test::serialize_common<test_simple_double_ptr::object<std::shared_ptr<double>>, sl::json_iarchive, sl::json_oarchive>::apply(obj);
    }
}

SL_ADD_TEST_CASE(simple_double_ptr_multiple_ptr)
{

    test_simple_double_ptr::object<double *> obj;
    obj.ptr() = new double{ 3. };

    std::vector<test_simple_double_ptr::observer> observers{ 10 };

    for (auto& v : observers)
    {
        v.link(obj.ptr());
    }

    auto all_object = std::make_tuple(std::move(obj), std::move(observers));

    sl::test::serialize_common<decltype(all_object), sl::json_iarchive, sl::json_oarchive>::apply(all_object);

}

SL_ADD_TEST_CASE(simple_double_ptr_array)
{

    test_simple_double_ptr_array::object obj;
    auto ptr = std::make_unique_for_overwrite<double[]>(5);

    for (std::size_t i = 0; i < 5; i++)
    {
        ptr[i] = static_cast<double>(i);
    }

    obj.ptr() = ptr.release();
    obj.size() = 5;

    sl::test::serialize_common<test_simple_double_ptr_array::object, sl::json_iarchive, sl::json_oarchive>::apply(obj);

}

SL_ADD_TEST_CASE(simple_double_ptr_multiple_ptr_array)
{

    test_simple_double_ptr_array::object obj;
    auto ptr = std::make_unique_for_overwrite<double[]>(5);

    for (std::size_t i = 0; i < 5; i++)
    {
        ptr[i] = static_cast<double>(i);
    }

    obj.ptr() = ptr.release();
    obj.size() = 5;

    std::vector<test_simple_double_ptr_array::observer> observers{ 10 };

    for (auto& v : observers)
    {
        v.link(obj.size(), obj.ptr());
    }

    auto all_object = std::make_tuple(std::move(obj), std::move(observers));

    sl::test::serialize_common<decltype(all_object), sl::json_iarchive, sl::json_oarchive>::apply(all_object);

}

SL_END_TEST_GROUP