#pragma once

#include <sstream>
#include "pointer_saver.h"

namespace sl
{

    namespace detail
    {
        template<typename T> struct helper;
        
    } // namespace detail
    
    class text_iarchive : public text_impl_iarchive<jump_space>, public detail::pointer_saver_in
    {
    public:
    
        ~text_iarchive() override = default;
        text_iarchive(std::istream &stream) : text_impl_iarchive(stream) {}

        template<typename T>
        text_iarchive &operator>>(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template deserialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        text_iarchive &operator&(T &&obj)
        {
            return *this >> std::forward<T>(obj);
        }
    };

    class text_oarchive : public text_impl_oarchive, public detail::pointer_saver_out
    {
    public:

        ~text_oarchive() override = default;
        text_oarchive(std::ostream &stream) : text_impl_oarchive(stream){}

        template<typename T>
        text_oarchive &operator<<(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template serialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        text_oarchive &operator&(T &&obj)
        {
            return *this << std::forward<T>(obj);
        }
    };
    
} // namespace sl

