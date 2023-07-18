namespace sl
{

    class access
    {
    public:

        template<typename Archive, typename T>
        static void serialize(Archive &ar, const T &obj, const unsigned int version)
        {
            obj.serialize(ar, version);
        }

        template<typename Archive, typename T>
        static void deserialize(Archive &ar, T &obj, const unsigned int version)
        {
            obj.deserialize(ar, version);
        }

        template<typename T>
        static constexpr auto & get_properties()
        {
            return T::properties;
        }

        template<typename, typename T>
        struct has_serialize
        {
            static_assert(std::integral_constant<T, false>::value, "The second template argument need to be a function");
        };

        template<typename, typename T>
        struct has_write
        {
            static_assert(std::integral_constant<T, false>::value, "The second template argument need to be a function");
        };

        template<typename, typename T>
        struct has_load
        {
            static_assert(std::integral_constant<T, false>::value, "The second template argument need to be a function");
        };

        template<typename, typename T>
        struct has_deserialize
        {
            static_assert(std::integral_constant<T, false>::value, "The second template argument need to be a function");
        };

        template<typename C, typename R, typename ...A>
        struct has_serialize<C, R(A...)>
        {

            template<typename T>
            static constexpr auto check(T *) -> 
                typename std::is_same<decltype(std::declval<T>().serialize(std::declval<A>()...)), R>::type;

            template<typename>
            static constexpr std::false_type check(...);

            typedef decltype(check<C>(0)) type;

        public:
            static constexpr bool value = type::value;
        };

        template<typename C, typename R, typename ...A>
        struct has_deserialize<C, R(A...)>
        {

            template<typename T>
            static constexpr auto check(T *) -> 
                typename std::is_same<decltype(std::declval<T>().deserialize(std::declval<A>()...)), R>::type;

            template<typename>
            static constexpr std::false_type check(...);

            typedef decltype(check<C>(0)) type;

        public:
            static constexpr bool value = type::value;
        };

    };
    
} // namespace sl