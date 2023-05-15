#pragma once

namespace sl
{

    namespace detail
    {
        
        template<typename T>
        inline T to_value(const char *);

        template<>
        inline double to_value(const char *value)
        {
            return std::atof(value);
        }


        template<>
        inline int to_value(const char *value)
        {
            return std::atoi(value);
        }

        template<>
        inline unsigned int to_value(const char *value)
        {
            char *end;
            return std::strtoul(value, &end, 10);
        }

        template<>
        inline long to_value(const char *value)
        {
            return std::atol(value);
        }

        template<>
        inline size_t to_value(const char *value)
        {
            char *end;
            return std::strtoull(value, &end, 10);
        }

        template<>
        inline bool to_value(const char *value)
        {
            return std::atoi(value);
        }

        template<>
        inline float to_value(const char* value)
        {
            return std::stof(value);
        }

    } // namepace detail
    
} // namespace sl
