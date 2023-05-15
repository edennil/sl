#pragma once

#include <map>
#include <vector>
#include <optional>

#include "basic.hpp"

namespace information
{
    namespace detail
    {
        struct null;

    } // namespace detail
    
} // namespace information

namespace sl
{
        
    namespace detail
    {

        struct avoid_property;

        template<bool, int I, int J>
        struct version_choice;

        template<int I, int J>
        struct version_choice<true, I, J>
        {
            constexpr static int value = I;
        };
        
        template<int I, int J>
        struct version_choice<false, I, J>
        {
            constexpr static int value = J;
        };

        template<int version, typename Archive> struct explicit_with_version;
        template<bool option, typename T> struct is_simple;
        template<int version> struct explicit_without_serialize_with_version;
        template<typename Archive, typename T> struct explicit_is_simple;
        template<typename Archive, typename T> struct explicit_helper;
        template<bool option> struct with_serialize;
        template<bool option, typename Ar> struct explicit_with_serialize;
        template<typename T> struct without_head;

        template<int version>
        struct with_version
        {
            template<typename Archive, typename T>
            static void serialize(Archive &out, const T &obj)
            {
                explicit_with_version<version, Archive>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive &in, T &obj)
            {
                explicit_with_version<version, Archive>::deserialize(in, obj);
            }

        };

        template<>
        struct with_version<0>
        {
            template<typename Archive, typename T>
            static void serialize(Archive &out, const T &obj)
            {
                explicit_with_version<0, Archive>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive &in, T &obj)
            {
                explicit_with_version<0, Archive>::deserialize(in, obj);
            }

        };


        template<int version>
        struct without_serialize_with_version
        {
            template<typename Archive, typename T>
            static void serialize(Archive &out, const T &obj)
            {
                explicit_without_serialize_with_version<version>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive &in, T &obj)
            {
                explicit_without_serialize_with_version<version>::deserialize(in, obj);
            }

        };

        template<>
        struct without_serialize_with_version<0>
        {
            template<typename Archive, typename T>
            static void serialize(Archive &out, const T &obj)
            {
                explicit_with_version<0, Archive>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive& in, T& obj)
            {
                explicit_with_version<0, Archive>::deserialize(in, obj);
            }

        };


        template<typename T>
        struct is_simple<true, T>
        {

            using working_type = std::decay_t<T>;

            template<typename Archive>
            static void serialize(Archive &out, const T &obj)
            {
                explicit_is_simple<Archive, working_type>::serialize(out, obj);
            }

            template<typename Archive, typename U>
            static void deserialize(Archive &in, U &obj)
            {
                explicit_is_simple<Archive, working_type>::deserialize(in, obj);
            }             

        };

        template<typename T>
        struct is_simple<false, T>
        {

            template<typename Archive>
            static void serialize(Archive &out, const T &obj)
            {
                static constexpr bool has_serialize = sl::access::template has_serialize<T, void(Archive &, const unsigned int)>::value;
                static constexpr bool is_polymorphic = std::is_polymorphic_v<std::decay_t<T>>;
                with_serialize<is_polymorphic || has_serialize>::serialize(out, obj);
            }

            template<typename Archive, typename U>
            static void deserialize(Archive &in, U &obj)
            {
                static constexpr bool has_deserialize = sl::access::template has_deserialize<U, void(Archive &, const unsigned int)>::value;
                static constexpr bool is_polymorphic = std::is_polymorphic_v<std::decay_t<U>>;
                with_serialize<is_polymorphic || has_deserialize>::deserialize(in, obj);
            }

        };

        template<>
        struct with_serialize<false>
        {

            template<typename Archive, typename T>
            static void serialize(Archive& out, const T& obj)
            {
                without_serialize_with_version<sl::detail::class_data<T>::version>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive& in, T& obj)
            {
                without_serialize_with_version<sl::detail::class_data<T>::version>::deserialize(in, obj);
            }

        };

        template<>
        struct with_serialize<true>
        {

            template<typename Archive, typename T>
            static void serialize(Archive& out, const T& obj)
            {
                explicit_with_serialize<true, Archive>::serialize(out, obj);
            }

            template<typename Archive, typename T>
            static void deserialize(Archive& in, T& obj)
            {
                explicit_with_serialize<true, Archive>::deserialize(in, obj);
            }
        };

        template<typename T>
        struct helper;

        template<typename T>
        struct helper
        {
            template<typename Archive, typename U>
            static void serialize(Archive& out, const U& obj)
            {
                is_simple<std::is_fundamental<U>::value || std::is_enum_v<U>, U>::serialize(out, obj);
            }

            template<typename Archive, typename U>
            static void deserialize(Archive& in, U&& obj)
            {
                using type = std::decay_t<U>;
                is_simple<std::is_fundamental<type>::value || std::is_enum_v<type>, type>::deserialize(in, std::forward<U>(obj));
            }
        };

        template<typename T, typename U, typename V, typename A>
        struct helper<std::tuple<sl::detail::array_property<T, U, V, A>, const char *, bool>>
        {
            using type = std::tuple<sl::detail::array_property<T, U, V, A>, const char *, bool>;

            template<typename Archive>
            static void serialize(Archive &out, const type &obj)
            {
                explicit_helper<Archive, type>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive& in, type&& obj)
            {
                explicit_helper<Archive, type>::deserialize(in, std::move(obj));
            }

        };

        template<typename T>
        struct helper<std::tuple<const T *, bool>>
        {
            using type = std::tuple<const T*, bool>;

            template<typename Archive>
            static void serialize(Archive &out, const type &obj)
            {
                explicit_helper<Archive, type>::serialize(out, obj);
            }
        };

        template<typename T>
        struct helper<std::tuple<T *, bool>>
        {
            using type = std::tuple<T*, bool>;

            template<typename Archive>
            static void deserialize(Archive &out, type &&obj)
            {
                explicit_helper<Archive, type>::deserialize(out, std::move(obj));
            }
        };

        template<typename P, typename T>
        struct helper<std::tuple<P, T *>>
        {
            using type = std::tuple<P, T*>;

            template<typename Archive>
            static void serialize(Archive &out, const type &obj)
            {
                explicit_helper<Archive, type>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, type &&obj)
            {
                explicit_helper<Archive, type>::deserialize(out, std::move(obj));
            }
        };

        template<>
        struct helper<std::string>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::string &obj)
            {
                explicit_helper<Archive, std::string>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, std::string &obj)
            {
                explicit_helper<Archive, std::string>::deserialize(out, obj);
            }
        };

        template<typename A>
        struct helper<std::vector<A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::vector<A> &obj)
            {
                explicit_helper<Archive, std::vector<A>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, std::vector<A> &obj)
            {
                explicit_helper<Archive, std::vector<A>>::deserialize(out, obj);
            }
        };

        template<typename K, typename V, typename C, typename A>
        struct helper<std::map<K, V, C, A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::map<K, V, C, A> &obj)
            {
                explicit_helper<Archive, std::map<K, V>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, std::map<K, V, C, A>& obj)
            {
                explicit_helper<Archive, std::map<K, V>>::deserialize(out, obj);
            }
        };

        template<typename K, typename V>
        struct helper<std::unordered_map<K, V>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::unordered_map<K, V> &obj)
            {
                explicit_helper<Archive, std::unordered_map<K, V>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, std::unordered_map<K, V> &obj)
            {
                explicit_helper<Archive, std::unordered_map<K, V>>::deserialize(out, obj);
            }
        };

        template<typename K, typename V, class H, class E, class A>
        struct helper<std::unordered_multimap<K, V, H, E, A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::unordered_multimap<K, V, H, E, A> &obj)
            {
                explicit_helper<Archive, std::unordered_multimap<K, V, H, E, A>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, std::unordered_multimap<K, V, H, E, A> &obj)
            {
                explicit_helper<Archive, std::unordered_multimap<K, V, H, E, A>>::deserialize(out, obj);
            }
        };

        template<typename ...A>
        struct helper<std::tuple<A...>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::tuple<A...> &obj)
            {
                explicit_helper<Archive, std::tuple<A...>>::serialize(out, obj);
            }

            template<typename Archive, typename U>
            static void deserialize(Archive &out, U &&obj)
            {
                explicit_helper<Archive, std::tuple<A...>>::deserialize(out, std::forward<U>(obj));
            }
        };

        template<typename A>
        struct helper<A *>
        {

            template<typename Archive>
            static void serialize(Archive &out, A *obj)
            {
                explicit_helper<Archive, A *>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &out, A *&obj)
            {
                explicit_helper<Archive, A *>::deserialize(out, obj);
            }
        };

        template<typename A>
        struct helper<without_head<A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const without_head<A> &obj)
            {
                explicit_helper<Archive, without_head<A>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &in, without_head<A> &obj)
            {
                explicit_helper<Archive, without_head<A>>::deserialize(in, obj);
            }
        };

        template<typename T>
        struct helper<std::tuple<avoid_property, T *, bool>>
        {

            template<typename Archive>
            static void deserialize(Archive &in, std::tuple<avoid_property, T *, bool> &&obj)
            {
                explicit_helper<Archive, avoid_property>::deserialize(in, std::get<0>(obj), std::get<1>(obj));
            }
        };

        template<typename T>
        struct helper<std::optional<T>>
        {
            template<typename Archive>
            static void serialize(Archive& out, const std::optional<T>& obj)
            {
                explicit_helper<Archive, std::optional<T>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive& in, std::optional<T>& obj)
            {
                explicit_helper<Archive, std::optional<T>>::deserialize(in, obj);
            }
        };

        template<typename A>
        struct helper<std::shared_ptr<A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::shared_ptr<A> &obj)
            {
                explicit_helper<Archive, std::shared_ptr<A>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &in, std::shared_ptr<A> &obj)
            {
                explicit_helper<Archive, std::shared_ptr<A>>::deserialize(in, obj);
            }
        };

        template<typename A>
        struct helper<std::unique_ptr<A>>
        {

            template<typename Archive>
            static void serialize(Archive &out, const std::unique_ptr<A> &obj)
            {
                explicit_helper<Archive, std::unique_ptr<A>>::serialize(out, obj);
            }

            template<typename Archive>
            static void deserialize(Archive &in, std::unique_ptr<A> &obj)
            {
                explicit_helper<Archive, std::unique_ptr<A>>::deserialize(in, obj);
            }
        };

        template<>
        struct helper<information::detail::null>
        {

            template<typename Archive>
            static void serialize(Archive &out, const information::detail::null &obj)
            {

            }

            template<typename Archive>
            static void deserialize(Archive &in, information::detail::null &obj)
            {

            }
        };
        
    } // namespace detail
    
} // namespace sl
