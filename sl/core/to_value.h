#pragma once

namespace sl
{

    namespace detail
    {
        
        template<typename T>
        static T to_value(const char *);

        template<>
        static double to_value(const char *value)
        {
            return std::atof(value);
        }


        template<>
        static int to_value(const char *value)
        {
            return std::atoi(value);
        }

        template<>
        static unsigned int to_value(const char *value)
        {
            char *end;
            return std::strtoul(value, &end, 10);
        }

        template<>
        static long to_value(const char *value)
        {
            return std::atol(value);
        }

        template<>
        static size_t to_value(const char *value)
        {
            char *end;
            return std::strtoull(value, &end, 10);
        }

        template<>
        static bool to_value(const char *value)
        {
            return std::atoi(value);
        }

    } // namepace detail
    
} // namespace sl
