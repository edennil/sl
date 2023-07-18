#pragma once

#include <iomanip>
#include <sl/core/json/helper_vector.hpp>
#include <sl/core/json/helper_map.hpp>
#include <sl/core/json/helper_tuple.hpp>

namespace sl
{
        
    namespace detail
    {

        template<int version, typename Archive> struct explicit_with_version;
        template<bool option, typename T> struct is_simple;
        template<int version> struct explicit_without_serialize_with_version;
        template<typename Archive, typename T> struct explicit_is_simple;
        template<typename Archive, typename T> struct explicit_helper;
        template<bool option> struct with_serialize;
        template<bool option, typename Ar> struct explicit_with_serialize;
        template<typename T> struct without_head;
        struct explicit_without_name;
        template<typename T> struct without_open;

        struct avoid_property;
        template<typename T> struct helper;

        namespace json
        {
            namespace pointer
            {

                struct explicit_helper_json
                {
                    template<typename PTR>
                    static void serialize_poly(json_oarchive &out, const PTR &obj)
                    {
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;
                        using memory_ptr = sl::patterns::singleton<sl::detail::builder<type>>;

                        auto id = out.id(obj);
                        if(id == -1)
                        {
                            id = out.add(obj);
                            const auto &child_name = memory_ptr::instance().name(ptr_wrapper<PTR>::get(obj));
                            auto version = memory_ptr::instance().version(ptr_wrapper<PTR>::get(obj));
                            auto tmp = std::string("{\"object\":\"") + class_data<type>::name + "\",\"@ptr\":" + std::to_string(id);
                            out.get() << tmp;
                            if(version != 0)
                            {
                                tmp = ",\"version\":" + std::to_string(version);
                                out.get() << tmp;
                            }
                            tmp = ",\"" + std::string(child_name) + "\":{";
                            out.get() << tmp;
                            memory_ptr::instance().save(ptr_wrapper<PTR>::get(obj), out);
                            out.get() << '}';
                            out.get() << '}';
                        }
                        else
                        {
                            auto tmp = std::string("{\"object\":\"") + class_data<type>::name + "\",\"@ptr\":" + std::to_string(id) + '}';
                            out.get() << tmp;
                        }
                    }

                    template<typename PTR>
                    static void deserialize_poly(json_iarchive &in, PTR &obj)
                    {
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;

                        auto extract_ptr = [](json_iarchive &in, PTR &obj)
                        {
                            int version = 0;
                            if(in.compare(",\"version\":", 11))
                            {
                                in += 11;
                                auto p = in.find_first_of(',');
                                auto size_version = p - in.position();
                                auto version_str = in.substr(size_version);
                                version = atoi(version_str.c_str());
                                in += static_cast<int>(size_version);
                            }
                            in++;
                            in++;
                            auto p = in.find_first_of('\"');
                            auto size_child = p - in.position();
                            auto child_name = in.substr(size_child);
                            in += static_cast<int>(size_child + 1);
                            in++;
                            in++;
                            sl::patterns::singleton<detail::builder<type>>::instance().build(child_name, version, in, obj);
                            in++;
                        };

                        const char *base_name = class_data<type>::name;
                        constexpr size_t len = class_data<type>::size;
                        in++;
                        in += 9;
                        in++;
                        in += 1 + len;
                        in++;
                        if(in.compare("\"@ptr\":", 7))
                        {
                            in += 6;
                            in++;
                            auto i = in.find_first_of(',');
                            auto j = in.find_first_of('}');
                            i = i > j ? j : i;
                            auto size_position = i - in.position();
                            auto position = std::atoi(in.substr(size_position).c_str());
                            in += static_cast<int>(size_position);
                            in.get_obj(position, obj);
                            if(obj == nullptr)
                            {
                                extract_ptr(in, obj);
                                in.add(position, obj);
                            }
                        }
                        else
                        {
                            in--;
                            extract_ptr(in, obj);
                        }
                        in++;
                    }

                    template<typename PTR>
                    static void serialize_simple(json_oarchive &out, const PTR &obj)
                    {
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;

                        auto id = out.id(obj);
                        if(id == -1)
                        {
                            id = out.add(obj);
                            out.get() << "{\"@ptr\":";
                            out.get() << std::to_string(id) + ",\"object\":";
                            helper<type>::serialize(out, *obj);
                            out.get() << '}';
                        }
                        else
                        {
                            out.get() << "{\"@ptr\":" << std::to_string(id) << '}';
                        }
                    }

                    template<typename PTR>
                    static void deserialize_simple(json_iarchive &in, PTR &obj)
                    {
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;

                        int id = -1;
                        if(in.compare("{\"@ptr\":", 8))
                        {
                            in += 8;
                            auto p = in.find_first_of(',');
                            auto id_size = p - in.position();
                            id = atoi(in.substr(id_size).c_str());
                            in += id_size + 10;
                        }
                        if(id == -1)
                        {
                            ptr_wrapper<PTR>::create(obj);
                            helper<type>::deserialize(in, *obj);
                        }
                        else
                        {
                            in.get_obj(id, obj);
                            if(!obj)
                            {
                                ptr_wrapper<PTR>::create(obj);
                                helper<type>::deserialize(in, *obj);
                                in.add(id, obj);
                            }
                            in++;
                        }
                    }

                    template<typename SIZE, typename PTR>
                    static void serialize_array(json_oarchive &out, SIZE size, const PTR &obj)
                    {
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;
                        auto id = out.id(obj);
                        if(id == -1)
                        {
                            id = out.add(obj);
                            out.get() << "{\"@ptr\":";
                            out.get() << std::to_string(id) + ",\"array\":[";
                            for(SIZE i = 0; i < size - 1; i++)
                            {
                                helper<type>::serialize(out, obj[i]);
                                out.get() << ',';
                            }
                            helper<type>::serialize(out, obj[size - 1]);
                            out.get() << ']';
                            out.get() << '}';
                        }
                        else
                        {
                            out.get() << "{\"@ptr\":" << std::to_string(id) << '}';
                        }
                    }

                    template<typename SIZE, typename PTR, typename ALLOCATOR>
                    static void deserialize_array(json_iarchive &in, SIZE size, PTR &obj)
                    {                   
                        using type = std::decay_t<typename ptr_wrapper<PTR>::type>;

                        auto deserialize = [](json_iarchive &in, SIZE size, PTR &obj)
                        {
                            in++;
                            for(SIZE i = 0; i < size; i++)
                            {
                                helper<type>::deserialize(in, obj[i]);
                                in++;
                            }
                        };

                        int id = -1;
                        if(in.compare("{\"@ptr\":", 8))
                        {
                            in += 8;
                            auto p = in.find_first_of(',');
                            auto id_size = p - in.position();
                            id = atoi(in.substr(id_size).c_str());
                            in += id_size + 9;
                        }
                        if(id == -1)
                        {
                            ptr_array<ALLOCATOR>::create(size, obj);
                            deserialize(in, size, obj);
                        }
                        else
                        {
                            in.get_obj(id, obj);
                            if(!obj)
                            {
                                ptr_array<ALLOCATOR>::create(size, obj);
                                deserialize(in, size, obj);
                                in.add(id, obj);                        
                            }
                            in++;
                        }
                    }

                };

            } // namespace pointer

            template<typename P>
            struct explicit_helper_property
            {
                using type = typename P::type;

                template<typename T>
                static void serialize(json_oarchive &out, const P &p, const T &obj)
                {
                    detail::helper<type>::serialize(out, obj.*(p.member_));
                }

                template<typename T>
                static void deserialize(json_iarchive &in, const P &p, T &obj)
                {
                    detail::helper<std::decay_t<type>>::deserialize(in, obj.*(p.member_));
                }
            };

            template<typename C, typename T, typename U, typename A>
            struct explicit_helper_property<array_property<C, T, U, A>>
            {
                using P = array_property<C, T, U, A>;

                template<typename V>
                static void serialize(json_oarchive &out, const P &p, const V &v)
                {
                    pointer::explicit_helper_json::serialize_array(out, v.*(p.member1_), v.*(p.member2_));
                }

                template<typename V>
                static void deserialize(json_iarchive &in, const P &p, V &v)
                {
                    pointer::explicit_helper_json::deserialize_array(in, v.*(p.member1_), v.*(p.member2_));
                }
            };
            
        } // namespace json

        template<>
        struct explicit_with_serialize<true, json_oarchive>
        {
            template<typename T>
            static void serialize(json_oarchive& out, const T& obj)
            {
                out.get() << '{';
                if (detail::class_data<T>::version != 0)
                {
                    std::string tmp("\"version\":");
                    out.get() << tmp;
                    out.get() << detail::class_data<T>::version;
                    out.get() << ',';
                }
                out.get() << '\"' + std::string(detail::class_data<T>::name) + "\":{";
                access::serialize(out, obj, detail::class_data<T>::version);
                out.get() << '}';
                out.get() << '}';
            }

        };

        template<>
        struct explicit_with_serialize<true, json_iarchive>
        {

            template<typename T>
            static void deserialize(json_iarchive &in, T &obj)
            {
                in++;
                int version(0);
                if(in.compare("\"version", 8))
                {
                    in +=10;
                    helper<int>::deserialize(in, version);
                    in++;
                }
                auto len = detail::class_data<T>::size;
                in++;
                if(!in.compare(detail::class_data<T>::name, len))
                {
                    throw std::exception();
                }
                in += len + 1;
                in++;
                in++;
                access::deserialize(in, obj, version);
                in++;
                in++;
            }
        };

        struct explicit_without_name
        {
            template<typename T>
            static void serialize(json_oarchive &out, const T &obj)
            {
                constexpr auto properties = access::template get_properties<T>();
                const auto size = std::tuple_size_v<decltype(properties)>;
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    auto property = std::get<i>(properties);
                    auto tmp = '\"' + std::string(property.name_) + '\"';
                    tmp += ':';
                    out.get() << tmp;
                    json::explicit_helper_property<decltype(property)>::serialize(out, property, obj);
                    if(i != size - 1)
                    {
                        out.get() << ',';
                    }
                });
            }
        };
        
        template<int version>
        struct explicit_without_serialize_with_version
        {

            template<typename T>
            static void serialize(json_oarchive &out, const T &obj)
            {
                out.get() << '{';
                out.get() << "\"version\":";
                out.get() << version;
                out.get() << ',';
                out.get() << '\"' + std::string(detail::class_data<T>::name) + "\":{";
                explicit_without_name::serialize(out, obj);
                out.get() << '}';
                out.get() << '}';
            }

            template<typename T>
            static void deserialize(json_iarchive& in, T& obj)
            {
                using type = std::decay_t<T>;
                in++;
                int local_version(0);
                if (in.compare("\"version", 8))
                {
                    in += 10;
                    helper<int>::deserialize(in, local_version);
                    in++;
                }
                if (local_version != version)
                {
                    throw std::exception();
                }
                auto len = detail::class_data<T>::size;
                in++;
                if (!in.compare(detail::class_data<T>::name, len))
                {
                    throw std::exception();
                }
                in += len + 1;
                in++;
                in++;
                constexpr auto properties = access::template get_properties<type>();
                const auto size = std::tuple_size_v<decltype(properties)>;
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    auto property = std::get<i>(properties);
                    in++;
                    size_t label_size = strlen(property.name_);
                    if (strncmp(in.c_str(), property.name_, label_size))
                    {
                        if (property.optional_)
                        {
                            if (i == size - 1)
                            {
                                in--;
                            }
                            in--;
                            return;
                        }
                        throw std::exception();
                    }
                    in += static_cast<int>(label_size + 2);
                    json::explicit_helper_property<decltype(property)>::deserialize(in, property, obj);
                    if (i != size - 1)
                    {
                        in++;
                    }
                    });
                in++;
                in++;
            }
        };

        template<>
        struct explicit_with_version<0, json_oarchive>
        {
            template<typename T>
            static void serialize(json_oarchive &out, const T &obj)
            {
                out.get() << '{';
                out.get() << '\"' + std::string(detail::class_data<T>::name) + "\":{";
                explicit_without_name::serialize(out, obj);
                out.get() << '}';
                out.get() << '}';
            }
        };

        template<>
        struct explicit_with_version<0, json_iarchive>
        {

            template<typename T>
            static void deserialize(json_iarchive& in, T&& obj)
            {
                using type = std::decay_t<T>;
                in++;
                in++;
                constexpr auto properties = access::template get_properties<type>();
                constexpr auto len = detail::class_data<type>::size;
                if (!in.compare(detail::class_data<type>::name, len))
                {
                    throw std::exception();
                }
                in += len + 2;
                in++;
                const auto size = std::tuple_size_v<decltype(properties)>;
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                    {
                        auto property = std::get<i>(properties);
                        in++;
                        size_t label_size = strlen(property.name_);
                        if (strncmp(in.c_str(), property.name_, label_size))
                        {
                            if (property.optional_)
                            {
                                if (i == size - 1)
                                {
                                    in--;
                                }
                                in--;
                                return;
                            }
                            throw std::exception();
                        }
                        in += static_cast<int>(label_size + 2);
                        json::explicit_helper_property<decltype(property)>::deserialize(in, property, obj);
                        if (i != size - 1)
                        {
                            in++;
                        }
                    });
                in++;
                in++;
            }
        };

        template<typename T, typename U, bool O>
        struct explicit_helper<json_oarchive, std::tuple<detail::basic_property<T, U, O>, const T *, bool>>
        {
            using type = std::tuple<detail::basic_property<T, U, O>, const T *, bool>;

            static void serialize(json_oarchive &out, const type &all_obj)
            {
                const auto &property = std::get<0>(all_obj);
                const auto &obj = std::get<1>(all_obj);
                const auto &is_end = std::get<2>(all_obj);
                using type = typename detail::basic_property<T, U, O>::type;
                auto tmp = '\"' + std::string(property.name_) + '\"';
                tmp += ':';
                out.get() << tmp;
                detail::helper<type>::serialize(out, (*obj).*(property.member_));
                if(!is_end)
                {
                    out.get() << ',';
                }
            }
        };

        template<typename T, typename U, typename V, typename A>
        struct explicit_helper<json_oarchive, std::tuple<detail::array_property<T, U, V, A>, const T *, bool>>
        {

            using type = std::tuple<detail::array_property<T, U, V, A>, const T *, bool>;

            static void serialize(json_oarchive &out, const type &all_obj)
            {
                const auto &property = std::get<0>(all_obj);
                using property_t = std::decay_t<decltype(property)>;
                const auto &obj = std::get<1>(all_obj);
                const auto &is_end = std::get<2>(all_obj);
                auto tmp = '\"' + std::string(property.name_) + '\"';
                tmp += ':';
                out.get() << tmp;
                json::explicit_helper_property<property_t>::serialize(out, property, obj);
                if(!is_end)
                {
                    out.get() << ',';
                }
            }
        };

        template<typename T, typename U, bool O>
        struct explicit_helper<json_iarchive, std::tuple<detail::basic_property<T, U, O>, T *, bool>>
        {
            using type = std::tuple<detail::basic_property<T, U, O>, T *, bool>;

            template<typename S>
            static void deserialize(json_iarchive &in, S &&all_obj)
            {
                const auto &property = std::get<0>(all_obj);
                const auto &obj = std::get<1>(all_obj);
                const auto &is_end = std::get<2>(all_obj);
                using property_t = typename sl::detail::basic_property<T, U, O>::type;
                in++;
                size_t label_size = strlen(property.name_);
                if(!in.compare(property.name_, label_size))
                {
                    if(O)
                    {
                        in--;
                        return;
                    }
                    throw std::exception();
                }\
                in += label_size + 1;
                in++;
                helper<property_t>::deserialize(in, (*obj).*(property.member_));
                if(!is_end)
                {
                    in++;
                }
            }
        };

        template<typename C, typename T, typename U, typename A>
        struct explicit_helper<json_iarchive, std::tuple<detail::array_property<C, T, U, A>, C *, bool>>
        {
            using type = std::tuple<detail::array_property<C, T, U, A>, C *, bool>;

            static void deserialize(json_iarchive &in, type &all_obj)
            {
                const auto &property = std::get<0>(all_obj);
                const auto &obj = std::get<1>(all_obj);
                const auto &is_end = std::get<2>(all_obj);
                using property_t = std::decay_t<decltype(property)>;
                in++;
                size_t label_size = strlen(property.name_);
                if(!in.compare(property.name_, label_size))
                {
                    throw std::exception();
                }
                in += label_size + 1;
                in++;
                json::explicit_helper_property<property_t>::deserialize(in, property, *obj);
                if(!is_end)
                {
                    in++;
                } 
            }
        };

        template<typename T>
        struct explicit_helper<json_oarchive, std::tuple<const T *, bool>>
        {

            static void serialize(json_oarchive &out, const std::tuple<const T *, bool> &all_obj)
            {
                const auto *obj = std::get<0>(all_obj);
                auto is_end = std::get<1>(all_obj);
                detail::explicit_helper<json_oarchive, without_open<T>>::serialize(out, *obj);
                if(!is_end)
                {
                    out.get() << ',';
                }
            }
        };

        template<typename T>
        struct explicit_helper<json_oarchive, without_open<T>>
        {
            static void serialize(json_oarchive &out, const T &obj)
            {
                if(detail::class_data<T>::version != 0)
                {
                    std::string tmp("\"version\":");
                    out.get() << tmp;
                    out.get() << detail::class_data<T>::version;
                    out.get() << ',';
                }
                out.get() << '\"' + std::string(detail::class_data<T>::name) + "\":{";
                access::serialize(out, obj, detail::class_data<T>::version);
                out.get() << '}';
            }
        };

        template<typename T>
        struct explicit_helper<json_iarchive, std::tuple<T *, bool>>
        {
            static void deserialize(json_iarchive &in, std::tuple<T *, bool> &&all_obj)
            {
                auto *obj = std::get<0>(all_obj);
                auto is_end = std::get<1>(all_obj);
                detail::explicit_helper<json_iarchive, without_open<T>>::deserialize(in, *obj);
                if(!is_end)
                {
                    in++;
                }
            }
        };

        template<typename T>
        struct explicit_helper<json_iarchive, without_open<T>>
        {
            static void deserialize(json_iarchive &in, T &obj)
            {
                int version(0);
                if(in.compare("\"version", 8))
                {
                    in += 10;
                    helper<int>::deserialize(in, version);
                    in++;
                }
                auto len = detail::class_data<T>::size;
                in++;
                if(!in.compare(detail::class_data<T>::name, len))
                {
                    throw std::exception();
                }
                in += len + 1;
                in++;
                in++;
                access::deserialize(in, obj, version);
                in++;
            }
        };

        template<typename P, typename T>
        struct explicit_helper<json_oarchive, std::tuple<P, T *>>
        {

            static void serialize(json_oarchive &out, const std::tuple<P, T *> &all_obj)
            {
                const auto &properties = std::get<0>(all_obj);
                const auto *obj = std::get<1>(all_obj);
                const auto size = std::tuple_size_v<P>;
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    const auto &property = std::get<i>(properties);
                    using property_t = std::decay_t<decltype(property)>;
                    auto tmp = '\"' + std::string(property.name_) + '\"';
                    tmp += ':';
                    out.get() << tmp;
                    json::explicit_helper_property<property_t>::serialize(out, property, *obj);
                    if(i != size - 1)
                    {
                        out.get() << ',';
                    }
                });
            }
        };

        template<typename P, typename T>
        struct explicit_helper<json_iarchive, std::tuple<P, T *>>
        {

            static void deserialize(json_iarchive &in, std::tuple<P, T *> &&all_obj)
            {
                const auto &properties = std::get<0>(all_obj);
                auto *obj = std::get<1>(all_obj);
                const auto size = std::tuple_size_v<P>;
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    const auto &property = std::get<i>(properties);
                    auto to_deserialize = std::make_tuple(property, obj, i != size - 1 ? false : true);
                    detail::helper<decltype(to_deserialize)>::deserialize(in, to_deserialize);
                });
            }
        };

        template<typename A>
        struct explicit_helper<json_oarchive, std::vector<A>>
        {
            static void serialize(json_oarchive& out, const std::vector<A>& obj)
            {
                constexpr static bool is_simple_value = std::is_fundamental_v<A> || std::is_same_v<A, std::string>;
                sl::detail::json::vector_tools::helper<is_simple_value>::serialize(out, obj);
            }
        };

        template<typename A>
        struct explicit_helper<json_iarchive, std::vector<A>>
        {
            static void deserialize(json_iarchive &out, std::vector<A> &obj)
            {
                constexpr static bool is_simple_value = std::is_fundamental_v<A> || std::is_same_v<A, std::string>;
                sl::detail::json::vector_tools::helper<is_simple_value>::deserialize(out, obj);
            }
        };

        template<>
        struct explicit_helper<json_oarchive, std::string>
        {
            static void serialize(json_oarchive &out, const std::string &obj)
            {
                out.get() << '\"' + obj + '\"';
            }
        };

        template<>
        struct explicit_helper<json_iarchive, std::string>
        {
            static void deserialize(json_iarchive &in, std::string &obj)
            {
                in++;
                auto i = in.find_first_of('\"');
                auto size = i - in.position();
                obj = in.substr(size);
                in += static_cast<int>(size + 1);
            }
        };

        template<typename K, typename V, typename H, typename E>
        struct explicit_helper<json_oarchive, std::unordered_map<K, V, H, E>>
        {
            static void serialize(json_oarchive& out, const std::unordered_map<K, V, H, E>& obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>;
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::serialize(out, obj);
            }
        };

        template<typename K, typename V, typename H, typename E>
        struct explicit_helper<json_iarchive, std::unordered_map<K, V, H, E>>
        {
            static void deserialize(json_iarchive& out, std::unordered_map<K, V, H, E>& obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>;
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::deserialize(out, obj);
            }
        };

        template<typename K, typename V, class H, class E, class A>
        struct explicit_helper<json_oarchive, std::unordered_multimap<K, V, H, E, A>>
        {
            static void serialize(json_oarchive& out, const std::unordered_multimap<K, V, H, E>& obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>;
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::serialize(out, obj);
            }
        };

        template<typename K, typename V, class H, class E, class A>
        struct explicit_helper<json_iarchive, std::unordered_multimap<K, V, H, E, A>>
        {
            static void deserialize(json_iarchive& out, std::unordered_multimap<K, V, H, E>& obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>;
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::deserialize(out, obj);
            }
        };

        template<typename K, typename V, typename C, typename A>
        struct explicit_helper<json_oarchive, std::map<K, V, C, A>>
        {
            static void serialize(json_oarchive &out, const std::map<K, V, C, A> &obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>; 
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::serialize(out, obj);
            }
        };

        template<typename K, typename V, typename C, typename A>
        struct explicit_helper<json_iarchive, std::map<K, V, C, A>>
        {
            static void deserialize(json_iarchive &out, std::map<K, V, C, A>&obj)
            {
                constexpr static bool is_simple_key = std::is_fundamental_v<K> || std::is_same_v<K, std::string>;
                constexpr static bool is_simple_value = std::is_fundamental_v<V> || std::is_same_v<V, std::string>; 
                sl::detail::json::map_tools::helper<is_simple_key, is_simple_value>::deserialize(out, obj);
            }
        };

        template<typename ...A>
        struct explicit_helper<json_oarchive, std::tuple<A...>>
        {
            static void serialize(json_oarchive &out, const std::tuple<A...> &obj)
            {
                sl::detail::json::tuple_tools::helper::serialize(out, obj);
            }
        };

        template<typename ...A>
        struct explicit_helper<json_iarchive, std::tuple<A...>>
        {
            static void deserialize(json_iarchive &in, std::tuple<A...> &obj)
            {
                sl::detail::json::tuple_tools::helper::deserialize(in, obj);
            }
        };

        template<typename A>
        struct explicit_helper<json_oarchive, std::optional<A>>
        {
            static void serialize(json_oarchive& out, const std::optional<A>& obj)
            {
                if (obj.has_value())
                {
                    detail::helper<A>::serialize(out, obj.value());
                }
                else
                {
                    out.get() << "null";
                }
            }
        };

        template<typename A>
        struct explicit_helper<json_iarchive, std::optional<A>>
        {
            static void deserialize(json_iarchive& in, std::optional<A>& obj)
            {
                if (!in.compare("null", 4))
                {
                    A a;
                    detail::helper<A>::deserialize(in, a);
                    obj = std::move(a);
                }
            }
        };

        template<typename...T>
        struct explicit_helper<json_oarchive, sl::detail::local_properties<T...>>
        {

            static void serialize(json_oarchive& out, const sl::detail::local_properties<T...>& obj)
            {
                constexpr auto size = sizeof...(T);
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    const auto& property = std::get<i>(obj.data_);
                    using property_t = std::decay_t<decltype(property)>;
                    helper<property_t>::serialize(out, property);
                    if (i != size - 1)
                    {
                        out.get() << ',';
                    }
                });
            }

        };

        template<typename...T>
        struct explicit_helper<json_iarchive, sl::detail::local_properties<T...>>
        {

            template<typename U>
            static void deserialize(json_iarchive& in, U&& obj)
            {
                constexpr auto size = sizeof...(T);
                detail::for_each(std::make_index_sequence<size>{}, [&](auto i)
                {
                    const auto& property = std::get<i>(obj.data_);
                    using property_t = std::decay_t<decltype(property)>;
                    helper<property_t>::deserialize(in, property);
                    if (i != size - 1)
                    {
                        in++;
                    }
                });
            }

        };

        template<typename Ar, bool>
        struct explicit_is_simple_enum;

        template<>
        struct explicit_is_simple_enum<json_oarchive, false>
        {
            template<typename T, std::enable_if_t<!std::is_floating_point_v<std::decay_t<T>>, int> * = nullptr>
            static void serialize(json_oarchive &out, T obj)
            {
                out.get() << obj;
            }

            template<typename T, std::enable_if_t<std::is_floating_point_v<std::decay_t<T>>, int> * = nullptr>
            static void serialize(json_oarchive &out, T obj)
            {
                out.get() << std::setprecision(16) << obj;
            }
        };

        template<>
        struct explicit_is_simple_enum<json_oarchive, true>
        {
            template<typename T>
            static void serialize(json_oarchive &out, T obj)
            {
                out.get() << obj;
            }
        };

        template<>
        struct explicit_is_simple_enum<json_iarchive, false>
        {

            template<typename T>
            static void deserialize(json_iarchive &in, T &obj)
            {
                auto i = in.find_first_of(',');
                auto j = in.find_first_of('}');
                auto k = in.find_first_of(']');
                i = i > j ? j : i;
                i = i > k ? k : i;
                auto size = i - in.position();
                auto value = in.substr(size);
                obj = sl::detail::to_value<T>(value.c_str());
                in += static_cast<int>(size);
            }
        };

        template<>
        struct explicit_is_simple_enum<json_iarchive, true>
        {

            template<typename T>
            static void deserialize(json_iarchive &in, T &obj)
            {
                auto i = in.find_first_of(',');
                auto j = in.find_first_of('}');
                auto k = in.find_first_of(']');
                i = i > j ? j : i;
                i = i > k ? k : i;
                auto size = i - in.position();
                auto value = in.substr(size);
                obj = static_cast<T>(atoi(value.c_str()));
                in += static_cast<int>(size);
            }
        };

        template<typename T>
        struct explicit_is_simple<json_oarchive, T>
        {
            template<typename U>
            static void serialize(json_oarchive &out, const U &obj)
            {
                explicit_is_simple_enum<json_oarchive, std::is_enum_v<std::decay_t<T>>>::serialize(out, obj);
            }
        };

        template<typename T>
        struct explicit_is_simple<json_iarchive, T>
        {
            template<typename U>
            static void deserialize(json_iarchive &in, U &obj)
            {
                explicit_is_simple_enum<json_iarchive, std::is_enum_v<std::decay_t<T>>>::deserialize(in, obj);
            }
        };

        template<typename T>
        struct explicit_helper<json_oarchive, without_head<T>>
        {
            static void serialize(json_oarchive &out, const without_head<T> &obj)
            {
                access::serialize(out, *obj.ptr_, obj.version_);
            }
        };

        template<typename T>
        struct explicit_helper<json_iarchive, without_head<T>>
        {
            static void deserialize(json_iarchive &in, without_head<T> &obj)
            {
                access::deserialize(in, *obj.ptr_, static_cast<unsigned int>(obj.version_));
            }
        };

        //template<>
        //struct explicit_helper<json_iarchive, avoid_property>
        //{
        //    static void deserialize(json_iarchive &in, const avoid_property &obj, bool is_last)
        //    {
        //        auto move = [&in](char open, char close)
        //        {
        //            int nb_open = 1;
        //            in++;
        //            auto p = in.find_first_of(close);
        //            while (nb_open)
        //            {
        //                auto q = in.find_first_of(open);
        //                if (p > q)
        //                {
        //                    nb_open++;
        //                    in += static_cast<int>(q - in.position() + 1);
        //                }
        //                else
        //                {
        //                    nb_open--;
        //                    in += static_cast<int>(p - in.position() + 1);
        //                    p = in.find_first_of(close);
        //                }
        //            }
        //        };
        //        in += static_cast<int>(strlen(obj.name_) + 3);
        //        if(in.compare('{'))
        //        {
        //            move('{', '}');
        //        }
        //        else if(in.compare('['))
        //        {
        //            move('[', ']');
        //        }
        //        else
        //        {
        //            auto i = in.find_first_of(',');
        //            auto j = in.find_first_of('}');
        //            auto k = in.find_first_of(']');
        //            i = i > j ? j : i;
        //            i = i > k ? k : i;
        //            auto size = i - in.position();
        //            in += static_cast<int>(size);                          
        //        }
        //        if(is_last)
        //        {
        //            in++;
        //        }
        //    }
        //};

        template<typename A>
        struct explicit_helper<json_oarchive, std::shared_ptr<A>>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive &out, const std::shared_ptr<F> &obj)
            {
                json::pointer::explicit_helper_json::serialize_poly(out, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive &out, const std::shared_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::serialize_simple(out, obj);
            }
        };


        template<typename A>
        struct explicit_helper<json_iarchive, std::shared_ptr<A>>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& out, std::shared_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::deserialize_poly(out, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& out, std::shared_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::deserialize_simple(out, obj);
            }
        };

        template<typename A>
        struct explicit_helper<json_oarchive, std::unique_ptr<A>>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive& out, const std::unique_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::serialize_poly(out, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive& out, const std::unique_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::serialize_simple(out, obj);
            }
        };


        template<typename A>
        struct explicit_helper<json_iarchive, std::unique_ptr<A>>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& out, std::shared_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::deserialize_poly(out, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& out, std::shared_ptr<F>& obj)
            {
                json::pointer::explicit_helper_json::deserialize_simple(out, obj);
            }
        };

        template<typename A>
        struct explicit_helper<json_oarchive, A*>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive& out, F* obj)
            {
                json::pointer::explicit_helper_json::serialize_poly(out, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void serialize(json_oarchive& out, F* obj)
            {
                json::pointer::explicit_helper_json::serialize_simple(out, obj);
            }
        };

        template<typename A>
        struct explicit_helper<json_iarchive, A*>
        {

            template<typename F, std::enable_if_t<std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& in, F*& obj)
            {
                json::pointer::explicit_helper_json::deserialize_poly(in, obj);
            }

            template<typename F, std::enable_if_t<!std::is_polymorphic_v<std::decay_t<F>>, int> = 0>
            static void deserialize(json_iarchive& in, F*& obj)
            {
                json::pointer::explicit_helper_json::deserialize_simple(in, obj);
            }
        };

    } // namespace detail
    
} // namespace gt
