#pragma once

#include <sl/tools/for_each.hpp>

namespace sl
{

    namespace tools
    {

        namespace detail
        {

            template<typename...T> class multi_iterator;

            template<typename... T>
            auto make_iter(T &&...t)
            {
                return multi_iterator<T...>(std::forward<T>(t)...);
            }

            namespace functions
            {

                struct add
                {

                    template<typename...T>
                    static void apply(T &&...t)
                    {
                        ((t++), ...);
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

                struct begin
                {
                    template<typename...T>
                    static auto apply(T &&...t)
                    {
                        return make_iter(t.begin()...);
                    }
                };

                struct end
                {
                    template<typename...T>
                    static auto apply(T &&...t)
                    {
                        return make_iter(t.end()...);
                    }
                };

                template<typename F>
                class calculate
                {
                public:

                    template<typename T>
                    static auto apply(T&& t)
                    {
                        constexpr auto size = std::tuple_size_v<std::decay_t<T>>;
                        return apply_impl(std::forward<T>(t), std::make_index_sequence<size>{});
                    }

                protected:

                    template<typename T, std::size_t...I>
                    static auto apply_impl(T&& t, std::index_sequence<I...>)
                    {
                        return F::apply(std::get<I>(t)...);
                    }
                };

            } // namespace functions

            template<typename T> using calculate = functions::calculate<T>;

            template<typename...T>
            class multi_iterator
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;

                using iterators = std::tuple<T...>;
                using data = std::tuple<typename T::reference...>;

                ~multi_iterator() = default;
                multi_iterator() = default;

                template<typename... U> multi_iterator(U &&...u) : iterators_(forward<T>(u)...) {}

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
                    calculate<functions::add>::apply(iterators_);
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
                    return calculate<functions::star>::apply(iterators_);
                }

                auto operator*() const
                {
                    return calculate<functions::star>::apply(iterators_);
                }


                iterators iterators_;
            };

            template<typename... T>
            class multi_container
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;

                ~multi_container() = default;
                multi_container() = delete;

                template<typename... U> multi_container(U &&...u) :data_(std::forward<T>(u)...) {}

                auto begin()
                {
                    return calculate<functions::begin>::apply(data_);
                }

                auto end()
                {
                    return calculate<functions::end>::apply(data_);
                }

                auto begin() const
                {
                    return calculate<functions::begin>::apply(data_);
                }

                auto end() const
                {
                    return calculate<functions::end>::apply(data_);
                }

            protected:

                std::tuple<T...> data_;
            };

        } // namespace detail

        template<typename... T>
        auto zip(T &&...t)
        {
            return detail::multi_container<T...>(std::forward<T>(t)...);
        }

    } // namespace tools

} // namespace sl