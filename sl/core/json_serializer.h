#pragma once

#include "text_impl.h"
#include "pointer_saver.h"

namespace sl
{
        
    namespace detail
    {
        template<typename T> struct helper;
    } // namespace detail
    

    class json_iarchive : public text_impl_iarchive<jump_space>, public detail::pointer_saver_in
    {
    public:
        ~json_iarchive() override = default;
        json_iarchive(std::istream &stream) : text_impl_iarchive(stream) {}

        template<typename T>
        json_iarchive &operator>>(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template deserialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        json_iarchive &operator&(T &&obj)
        {
            return *this >> std::forward<T>(obj);
        }
    };

    class json_oarchive : public text_impl_oarchive, public detail::pointer_saver_out
    {
    public:
        ~json_oarchive() override = default;
        json_oarchive(std::ostream &stream) : text_impl_oarchive(stream){}

        template<typename T>
        json_oarchive &operator<<(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template serialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        json_oarchive &operator&(T &&obj)
        {
            return *this << std::forward<T>(obj);
        }
    };
   
} // namespace sl
