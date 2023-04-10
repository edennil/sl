#pragma once

namespace sl
{

    namespace tools
    {

        template<typename T, T...U, typename F>
        constexpr void for_each(std::integer_sequence<T, U...>, F&& f)
        {
            using tmp = int[];
            (void)tmp {
                (static_cast<void>(f(std::integral_constant<T, U>{})), 0)..., 0
            };
        } 

    } // namespace tools

} // namespace sl
