#pragma once

namespace sl
{

    namespace detail
    {
        
        template<typename T>
        T to_value(const char *);

        template<>
        double to_value(const char *value)
        {
            return std::atof(value);
        }


        template<>
        int to_value(const char *value)
        {
            return std::atoi(value);
        }

        template<>
        unsigned int to_value(const char *value)
        {
            char *end;
            return std::strtoul(value, &end, 10);
        }

        template<>
        long to_value(const char *value)
        {
            return std::atol(value);
        }

        template<>
        size_t to_value(const char *value)
        {
            char *end;
            return std::strtoull(value, &end, 10);
        }

        template<>
        bool to_value(const char *value)
        {
            return std::atoi(value);
        }

    } // namepace detail
    
} // namespace sl
