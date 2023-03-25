#pragma once

#include "helper_fundamental.h"

namespace sl
{

    namespace detail
    {

        namespace json
        {

            namespace vector_tools
            {

                template<bool VALUE>
                struct helper;

                template<>
                struct helper<true>
                {

                    template<typename container>
                    static void serialize(json_oarchive& out, const container& obj)
                    {
                        using value_type = typename container::value_type;

                        out.get() << '[';
                        if (!obj.empty())
                        {
                            for (std::size_t i = 0; i < obj.size() - 1; i++)
                            {
                                gt::serialization::detail::json::fundamenta_tools::helper<value_type>::serialize(out, obj[i]);
                                out.get() << ',';
                            }
                            gt::serialization::detail::json::fundamenta_tools::helper<value_type>::serialize(out, obj[obj.size() - 1]);
                        }
                        out.get() << ']';
                    }

                    template<typename container>
                    static void deserialize(json_iarchive& in, container& obj)
                    {
                        using value_type = typename container::value_type;

                        in++;
                        if (in.compare(']'))
                        {
                            in++;
                            return;
                        }
                        while (true)
                        {
                            value_type value;
                            gt::serialization::detail::json::fundamenta_tools::helper_in_vector<value_type>::deserialize(in, value);
                            obj.push_back(std::move(value));
                            if (in.compare(']'))
                            {
                                in++;
                                break;
                            }
                            in++;
                        }
                    }
                };

                template<>
                struct helper<false>
                {

                    template<typename container>
                    static void serialize(json_oarchive& out, const container& obj)
                    {
                        using value_type = typename container::value_type;

                        out.get() << '[';
                        if (!obj.empty())
                        {
                            for (std::size_t i = 0; i < obj.size() - 1; i++)
                            {
                                gt::serialization::detail::helper<value_type>::serialize(out, obj[i]);
                                out.get() << ',';
                            }
                            gt::serialization::detail::helper<value_type>::serialize(out, obj[obj.size() - 1]);
                        }
                        out.get() << ']';
                    }

                    template<typename container>
                    static void deserialize(json_iarchive& in, container& obj)
                    {
                        using value_type = typename container::value_type;

                        in++;
                        if (in.compare(']'))
                        {
                            in++;
                            return;
                        }
                        while (true)
                        {
                            value_type value;
                            gt::serialization::detail::helper<value_type>::deserialize(in, value);
                            obj.push_back(std::move(value));
                            if (in.compare(']'))
                            {
                                in++;
                                break;
                            }
                            in++;
                        }
                    }
                };

            } // namespace vector_tools

        } // namespace json

    } // namespace detail

} // namespace sl