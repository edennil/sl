#pragma once

#include <sl/tools/for_each.hpp>

namespace sl
{

    namespace tools
    {

        namespace detail
        {

            struct add_pp
            {
                template<typename...T>
                static void apply(T &&...t)
                {
                    ((t++),...);
                }
            };

            struct star
            {
                template<typename...T>
                static auto apply(T &&...t)
                {
                    return std::forward_as_tuple(*t ...);
                }
            };

            template<typename F, typename D, std::size_t... I>
            auto calculate(D &&d, std::index_sequence<I...>)
            {
                return F::apply(std::get<I>(d)...);
            }

            template<typename...T>
            class multi_iterator
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;

                using iterators = std::tuple<T...>;
                using data = std::tuple<typename T::reference...>;

                ~multi_iterator() = default;
                multi_iterator() = default;

                template<typename... U>
                multi_iterator(U &&...u)
                    :iterators_(forward<T>(u)...)
                {

                }

                bool operator!=(const multi_iterator &rhs) const
                {
                    bool res = true;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        res &= std::get<i>(iterators_) != std::get<i>(rhs.iterators_);
                        });
                    return res;
                }

                multi_iterator& operator++()
                {
                    calculate<add_pp>(iterators_, std::make_index_sequence<size>{});
                    return *this;
                }

                multi_iterator operator++(int)
                {
                    multi_iterator it;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(it.iterators_) = std::get<i>(iterators_);
                        std::get<i>(iterators_)++;
                        });
                    return it;
                }

                auto operator*()
                {
                    return calculate<star>(iterators_, std::make_index_sequence<size>{});
                }

                auto operator*() const
                {
                    return calculate<star>(iterators_, std::make_index_sequence<size>{});
                }


                iterators iterators_;
            };

            template<typename... T>
            auto make_iter(T &&...t)
            {
                return multi_iterator<T...>(std::forward<T>(t)...);
            }

            struct begin_function
            {
                template<typename...T>
                static auto apply(T &&...t)
                {
                    return make_iter(t.begin()...);
                }
            };

            struct end_function
            {
                template<typename...T>
                static auto apply(T &&...t)
                {
                    return make_iter(t.end()...);
                }
            };

            template<typename... T>
            class multi_container
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;

                ~multi_container() = default;
                multi_container() = delete;

                template<typename... U>
                multi_container(U &&...u)
                :data_(std::forward<T>(u)...)
                {

                }

                auto begin()
                {
                    return calculate<begin_function>(data_, std::make_index_sequence<size>{});
                }

                auto end()
                {
                    return calculate<end_function>(data_, std::make_index_sequence<size>{});
                }

                auto begin() const
                {
                    return calculate<begin_function>(data_, std::make_index_sequence<size>{});
                }

                auto end() const
                {
                    return calculate<end_function>(data_, std::make_index_sequence<size>{});
                }

            protected:

                std::tuple<T...> data_;
            };

        } // namespace detail

        template<typename... T>
        auto make_multi(T &&...t)
        {
            return detail::multi_container<T...>(std::forward<T>(t)...);
        }

    } // namespace tools

} // namespace sl