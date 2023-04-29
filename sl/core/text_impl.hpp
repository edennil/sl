#pragma once

#include <iostream>
#include <cstring>

namespace sl
{

    namespace detail
    {

        class text_impl_iarchive_base
        {
        public:

            virtual ~text_impl_iarchive_base() = default;

            text_impl_iarchive_base(std::istream &stream) : stream_(stream) 
            {
                values_ = std::string(std::istreambuf_iterator<char>(stream), {});
                index_ = 0;
            }

            std::string substr(size_t length) const
            {
                return values_.substr(index_, length);
            }

            size_t position() const
            {
                return index_;
            }

            size_t find_first_of(char value) const
            {
                return values_.find_first_of(value, index_);
            }

            size_t find(const char *value, size_t size)
            {
                return values_.find(value, index_, size);
            }

            bool compare(char c)
            {
                return values_.c_str()[index_] == c;
            }

            bool compare(const char *value, size_t size)
            {
                if(strncmp(&values_[index_], value, size))
                {
                    return false;
                }
                return true;
            }

            const char *c_str() const
            {
                return &values_[index_];
            }

        protected:
            std::istream &stream_;
            std::string values_;
            size_t index_;
        };

    } // namespace detail

    struct jump_space {};
    struct non_jump_space {};

    template<typename JUMP>
    class text_impl_iarchive;

    template<>
    class text_impl_iarchive<jump_space> : public detail::text_impl_iarchive_base
    {
    public:

        virtual ~text_impl_iarchive() = default;

        text_impl_iarchive(std::istream &stream) : text_impl_iarchive_base::text_impl_iarchive_base(stream){}

        text_impl_iarchive &operator++(int)
        {
            index_++;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n' || tmp_values[index_] == ' ')
            {
                index_++;
            }
            return *this;
        }

        text_impl_iarchive &operator--(int)
        {
            index_--;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n' || tmp_values[index_] == ' ')
            {
                index_--;
            }
            return *this;
        }

        text_impl_iarchive &operator+=(size_t value)
        {
            index_ += value;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n' || tmp_values[index_] == ' ')
            {
                index_++;
            }
            return *this;
        }

    };

    template<>
    class text_impl_iarchive<non_jump_space> : public detail::text_impl_iarchive_base
    {
    public:

        virtual ~text_impl_iarchive() = default;

        text_impl_iarchive(std::istream &stream) : text_impl_iarchive_base::text_impl_iarchive_base(stream){}

        text_impl_iarchive &operator++(int)
        {
            index_++;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n' || tmp_values[index_] == ' ')
            {
                index_++;
            }
            index_--;
            return *this;
        }

        text_impl_iarchive &operator--(int)
        {
            index_++;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n')
            {
                index_--;
            }
            index_--;
            return *this;
        }

        text_impl_iarchive &operator+=(size_t value)
        {
            index_ += value;
            const char *tmp_values = values_.c_str();
            while(tmp_values[index_] == '\t' || tmp_values[index_] == '\n')
            {
                index_++;
            }
            return *this;
        }  
    };

    class text_impl_oarchive
    {
    public:

        virtual ~text_impl_oarchive() = default;

        text_impl_oarchive(std::ostream &stream) : stream_(stream) {}

        std::ostream & get()
        {
            return stream_;
        }

    protected:
        std::ostream &stream_;
    };

} // namespace sl