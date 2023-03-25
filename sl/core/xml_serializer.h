#pragma once

#include <sstream>
#include "pointer_saver.h"
#include "text_impl.h"

namespace sl
{

    namespace detail
    {
        template<typename T> struct helper;
        
    } // namespace detail
    
    class xml_iarchive : public text_impl_iarchive<jump_space>, public detail::pointer_saver_in
    {
    public:
        ~xml_iarchive() override = default;
        xml_iarchive(std::istream &stream) : text_impl_iarchive(stream) {
            if(!values_.empty())
            {
                if(compare("<?", 2))
                {
                    auto it = find("?>", 2);
                    index_ += it + 2;
                    while(values_.c_str()[index_] == '\n')
                    {
                        index_++;
                    }
                }
            }

        }

        template<typename T>
        xml_iarchive &operator>>(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template deserialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        xml_iarchive &operator&(T &&obj)
        {
            return *this >> std::forward<T>(obj);
        }
    };

    class xml_oarchive : public text_impl_oarchive, public detail::pointer_saver_out
    {
    public:
    
        ~xml_oarchive() override = default;
        xml_oarchive(std::ostream &stream) : text_impl_oarchive(stream){}

        template<typename T>
        xml_oarchive &operator<<(T &&obj)
        {
            using type = std::decay_t<T>;
            detail::helper<type>::template serialize(*this, std::forward<T>(obj));
            return *this;
        }

        template<typename T>
        xml_oarchive &operator&(T &&obj)
        {
            return *this << std::forward<T>(obj);
        }
    };
    
} // namespace sl

