#pragma once

#include<sstream>
#include "pointer_saver.hpp"

namespace sl
{

    namespace detail
    {

        template<typename T> struct helper;
        
    } // namespace detail
    
    class binary_iarchive : public detail::pointer_saver_in
    {
    public:

        ~binary_iarchive() = default;
        binary_iarchive(std::istream& stream) : stream_(stream)
        {
            buf_ = stream_.rdbuf();
        }

        template<typename T>
        binary_iarchive& operator>>(T&& obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template deserialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        binary_iarchive& operator&(T&& obj)
        {
            return *this >> std::forward<T>(obj);
        }

        std::istream& get()
        {
            return stream_;
        }

        std::streambuf* buff()
        {
            return buf_;
        }

    protected:

        std::istream& stream_;
        std::streambuf* buf_;
    };

    class binary_oarchive : public detail::pointer_saver_out
    {
    public:

        virtual ~binary_oarchive() = default;
        binary_oarchive(std::ostream& stream) : stream_(stream)
        {
            buf_ = stream_.rdbuf();
        }

        template<typename T>
        binary_oarchive& operator<<(T&& obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template serialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        binary_iarchive& operator&(T&& obj)
        {
            return *this << std::forward<T>(obj);
        }

        std::ostream& get()
        {
            return stream_;
        }

        std::streambuf* buff()
        {
            return buf_;
        }

    protected:

        std::ostream& stream_;
        std::streambuf * buf_;
    };
        
} // namespace sl
