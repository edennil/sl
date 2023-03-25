#pragma once

#include "helper_fundamental.h"

namespace sl
{

    namespace detail
    {

        namespace json
        {

            namespace tuple_tools
            {

                struct helper
                {

                    template<typename container>
                    static void serialize(json_oarchive &out, const container &obj)
                    {
                        static constexpr auto tuple_size = std::tuple_size_v<container>;
                        static constexpr auto last = tuple_size - 1;
                        out.get() << '[';

                        detail::for_each(std::make_index_sequence<last>{}, [&](auto i) {
                            using value_type = std::tuple_element_t<i, container>;
                            const auto& value = std::get<i>(obj);
                            static constexpr bool is_simple = std::is_fundamental_v<value_type> || std::is_same_v<value_type, std::string>;
                            if constexpr (is_simple)
                            {
                                gt::serialization::detail::json::fundamenta_tools::helper<value_type>::serialize(out, value);
                            }
                            else
                            {
                                gt::serialization::detail::helper<value_type>::serialize(out, value);
                            }
                            out.get() << ',';
                        });

                        using value_type = std::tuple_element_t<last, container>;
                        const auto& value = std::get<last>(obj);
                        static constexpr bool is_simple = std::is_fundamental_v<value_type> || std::is_same_v<value_type, std::string>;
                        if constexpr (is_simple)
                        {
                            gt::serialization::detail::json::fundamenta_tools::helper<value_type>::serialize(out, value);
                        }
                        else
                        {
                            gt::serialization::detail::helper<value_type>::serialize(out, value);
                        }
                        out.get() << ']';
                    }

                    template<typename container>
                    static void deserialize(json_iarchive& in, const container& obj)
                    {
                        static constexpr auto tuple_size = std::tuple_size_v<container>;
                        static constexpr auto last = tuple_size - 1;
                        in++;

                        detail::for_each(std::make_index_sequence<last>{}, [&](auto i) {
                            using value_type = std::tuple_element_t<i, container>;
                            auto& value = std::get<i>(obj);
                            static constexpr bool is_simple = std::is_fundamental_v<value_type> || std::is_same_v<value_type, std::string>;
                            if constexpr (is_simple)
                            {
                                gt::serialization::detail::json::fundamenta_tools::helper<value_type>::deserialize(in, value);
                            }
                            else
                            {
                                gt::serialization::detail::helper<value_type>::deserialize(in, value);
                            }
                            in++;
                            });

                        using value_type = std::tuple_element_t<last, container>;
                        const auto& value = std::get<last>(obj);
                        static constexpr bool is_simple = std::is_fundamental_v<value_type> || std::is_same_v<value_type, std::string>;
                        if constexpr (is_simple)
                        {
                            gt::serialization::detail::json::fundamenta_tools::helper<value_type>::deserialize(in, value);
                        }
                        else
                        {
                            gt::serialization::detail::helper<value_type>::deserialize(in, value);
                        }
                        in++;
                    }
                };

            } // namespace tuple_tools

        } // namespace json

    } // namespace detail

} // namespace sl