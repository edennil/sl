#pragma once

#include <sl/tools/for_each.hpp>

namespace sl
{

    namespace tools
    {

        namespace detail
        {

            template<typename...T>
            class multi_iterator
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;

                using iterators = std::tuple<T...>;
                using data = std::tuple<typename T::reference...>;

                ~multi_iterator() = default;
                multi_iterator() = default;

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
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(iterators_)++;
                        });
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

                iterators & operator*()
                {
                    return iterators_;
                }

                const iterators& operator*() const
                {
                    return iterators_;
                }


                iterators iterators_;
            };

            template<typename... T>
            class multi_container
            {
            public:

                constexpr static std::size_t size = std::tuple_size_v<std::tuple<T...>>;
                using iterator = multi_iterator<typename std::decay_t<T>::iterator...>;
                using const_iterator = multi_iterator<typename std::decay_t<T>::const_iterator...>;

                ~multi_container() = default;
                multi_container() = delete;

                template<typename... U>
                multi_container(U &&...u)
                :data_(std::forward<T>(u)...)
                {

                }

                iterator begin()
                {

                    iterator it;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(it.iterators_) = std::get<i>(data_).begin();
                        });
                    return it;
                }

                iterator end()
                {
                    iterator it;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(it.iterators_) = std::get<i>(data_).end();
                        });
                    return it;
                }

                const_iterator begin() const
                {
                    const_iterator it;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(it.iterators_) = std::get<i>(data_).begin();
                        });
                    return it;
                }

                const_iterator end() const
                {
                    const_iterator it;
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i) {
                        std::get<i>(it.iterators_) = std::get<i>(data_).end();
                        });
                    return it;
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