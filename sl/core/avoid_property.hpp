#pragma once

namespace sl
{
    namespace detail
    {
        struct avoid_property
        {
            constexpr avoid_property(const char *name) : name_{name} {}
            const char *name_;
        };
        
    } // namespace detail
    
} // namespace sl