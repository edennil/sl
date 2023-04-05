#pragma once

#include "helper_fundamental.hpp"

namespace sl
{

    namespace detail
    {

        namespace json
        {

            namespace map_tools
            {

                template<bool KEY, bool VALUE>
                struct helper;

                template<>
                struct helper<true, true>
                {

                    template<typename container>
                    static void serialize(json_oarchive& out, const container& obj)
                    {

                        using key_type = typename container::key_type;
                        using mapped_type = typename container::mapped_type;

                        out.get() << '{';
                        if (!obj.empty())
                        {
                            auto size = obj.size();
                            size_t j = 0;
                            for (const auto &m : obj)
                            {
                                sl::detail::json::fundamenta_tools::helper<key_type>::serialize(out, m.first);
                                out.get() << ':';
                                sl::detail::json::fundamenta_tools::helper<mapped_type>::serialize(out, m.second);
                                j++;
                                if (j < size)
                                {
                                    out.get() << ',';
                                }
                            }
                        }
                        out.get() << '}';
                    }

                    template<typename container>
                    static void deserialize(json_iarchive& in, container& obj)
                    {
                        using key_type = typename container::key_type;
                        using mapped_type = typename container::mapped_type;

                        in++;
                        while (true)
                        {
                            key_type key;
                            mapped_type value;
                            sl::detail::json::fundamenta_tools::helper_key_in_map<key_type>::deserialize(in, key);
                            in++;
                            sl::detail::json::fundamenta_tools::helper_in_vector<mapped_type>::deserialize(in, value);
                            obj.emplace(std::move(key), std::move(value));
                            if (in.compare('}'))
                            {
                                in++;
                                break;
                            }
                            in++;
                        }
                    }

                };

                template<>
                struct helper<true, false>
                {

                    template<typename container>
                    static void serialize(json_oarchive& out, const container& obj)
                    {

                        using key_type = typename container::key_type;
                        using mapped_type = typename container::mapped_type;

                        out.get() << '{';
                        if (!obj.empty())
                        {
                            auto size = obj.size();
                            size_t j = 0;
                            for (const auto& m : obj)
                            {
                                sl::detail::json::fundamenta_tools::helper<key_type>::serialize(out, m.first);
                                out.get() << ':';
                                sl::detail::helper<mapped_type>::serialize(out, m.second);
                                j++;
                                if (j < size)
                                {
                                    out.get() << ',';
                                }
                            }
                        }
                        out.get() << '}';
                    }

                    template<typename container>
                    static void deserialize(json_iarchive& in, const container& obj)
                    {
                        using key_type = typename container::key_type;
                        using mapped_type = typename container::mapped_type;

                        in++;
                        while (true)
                        {
                            key_type key;
                            mapped_type value;
                            sl::detail::json::fundamenta_tools::helper<key_type>::deserialize(in, key);
                            in++;
                            sl::detail::helper<mapped_type>::deserialize(in, value);
                            obj.emplace(std::move(key), std::move(value));
                            if (in.compare('}'))
                            {
                                in++;
                                break;
                            }
                            in++;
                        }
                    }

                };

            } // namespace map_tools

        } // namespace json

    } // namespace detail

} // namespace sl