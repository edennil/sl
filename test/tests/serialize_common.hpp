
#include <sl/core/basic.hpp>

namespace sl
{

    namespace test
    {
        template<typename...T>
        bool operator==(const std::tuple<T...> &left, const std::tuple<T...>& right)
        {
            bool res = true;
            constexpr auto size = sizeof...(T);
            sl::detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
            {
                res &= (std::get<i>(left) == std::get<i>(right));
            });
            return res;
        }

        template<typename T, typename IN, typename OUT>
        struct serialize_common
        {
            static void apply(const T& original)
            {
                T copy;
                std::stringstream stream;
                sl::save<std::stringstream, OUT, T>(stream, original);
                sl::load<std::stringstream, IN, T>(stream, copy);
                original == copy;
            }
        };

    } // namespace test

} // namespace sl