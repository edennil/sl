#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <optional>

#include <sl/macros/IIF.h>
#include <sl/macros/NARG.h>
#include <sl/macros/EQUAL.h>
#include <sl/tools/zip.hpp>
#include <sl//tools/for_each.hpp>

#include <type_traits>

#define _OPERATORS_(OP)  template<typename Left, typename Right> auto operator OP(const variable<Left>& l, Right&& r){auto tmp = l.value_ OP r; return variable<decltype(tmp)>(tmp);}

#define _OPERATORS_EXPR_(OP, type) template<typename Left, typename Right> expr<variable<Left>, Right, type> operator OP(const variable<Left>& l, Right&& r) { return expr<variable<Left>, Right, type>(l, std::forward<Right>(r));}

#define _CALCULUS_(type, OP, inv_OP)         template<> struct calculus<type> { template<typename Left, typename Right> static bool apply(const Left& left, const Right& right){ return left OP right;} \
                                                static const std::string &inverse(){static std::string label(inv_OP);return label;}}

namespace sl
{

    namespace test
    {

        namespace detail
        {

            struct traits_base
            {
                constexpr static bool is_boolean = false;
                constexpr static bool is_arithmetic = false;
                constexpr static bool is_string = false;
                constexpr static bool is_container = false;
                constexpr static bool is_key_container = false;
                constexpr static bool is_tuple = false;
                constexpr static bool is_optional = false;
            };

            template<typename T>
            struct traits : public traits_base
            {
                constexpr static bool is_arithmetic = std::is_arithmetic_v<T>;
            };

            template<>
            struct traits<bool> : public traits_base
            {
                constexpr static bool is_boolean = true;
            };

            template<class C, class T, class A>
            struct traits<std::basic_string<C, T, A>> : public traits_base
            {
                constexpr static bool is_string = true;
            };

            template<typename T, typename A>
            struct traits<std::vector<T, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
            };

            template<typename T, typename A>
            struct traits<std::list<T, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
            };

            template<typename K, typename C, typename A>
            struct traits<std::set<K, C, A>> : public traits_base
            {
                using value_type = K;
                constexpr static bool is_container = true;
            };

            template<typename K, typename T, typename C, typename A>
            struct traits<std::map<K, T, C, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
                constexpr static bool is_key_container = true;
            };

            template<typename K, typename T, typename H, typename C, typename A>
            struct traits<std::unordered_map<K, T, H, C, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
                constexpr static bool is_key_container = true;
            };

            template<typename K, typename T, typename C, typename A>
            struct traits<std::multimap<K, T, C, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
                constexpr static bool is_key_container = true;
            };

            template<typename K, typename T, typename H, typename C, typename A>
            struct traits<std::unordered_multimap<K, T, H, C, A>> : public traits_base
            {
                using value_type = T;
                constexpr static bool is_container = true;
                constexpr static bool is_key_container = true;
            };

            template<typename... T>
            struct traits<std::tuple<T...>> : public traits_base
            {
                constexpr static bool is_tuple = true;
            };

            template<typename T>
            struct traits<std::optional<T>> : public traits_base
            {
                constexpr static bool is_optional = true;
            };

            template<typename Left, typename Right>
            struct traits_double
            {

                using traits_left = traits<std::decay_t<Left>>;
                using traits_right = traits<std::decay_t<Right>>;

                constexpr static bool is_arithmetic = traits_left::is_arithmetic && traits_right::is_arithmetic;
                constexpr static bool is_boolean = traits_left::is_boolean && traits_right::is_boolean;
                constexpr static bool is_string = traits_left::is_string && traits_right::is_string;
                constexpr static bool is_container = traits_left::is_container && traits_right::is_container;
                constexpr static bool is_key_container = traits_left::is_key_container && traits_right::is_key_container;
                constexpr static bool is_tuple = traits_left::is_tuple && traits_right::is_tuple;
                constexpr static bool is_optional = traits_left::is_optional && traits_right::is_optional;
            };

        } // namespace detail

        template<typename Left, typename Right> using traits_double = detail::traits_double<Left, Right>;

        inline double& tolerance()
        {
            static double eps = 0.;
            return eps;
        }


        template<typename Left, typename Right, std::enable_if_t<traits_double<Left, Right>::is_arithmetic, int>* = nullptr>
        std::tuple<bool, std::string> compare_with_tolerance(const Left& left, const Right& right)
        {
            auto eps = tolerance();
            auto diff = fabs(left - right);
            auto fleft = diff / left;
            auto fright = diff / right;
            auto res = std::max(fleft, fright);
            if (res > eps)
            {
                std::string message = "relative difference exceeds tolerance [" + std::to_string(res) + " > " + std::to_string(eps) + ']';
                return std::make_tuple(false, std::move(message));
            }
            return std::make_tuple(true, std::string());
        }

        template<typename Left, typename Right, std::enable_if_t<traits_double<Left, Right>::is_string || traits_double<Left, Right>::is_boolean, int>* = nullptr>
        std::tuple<bool, std::string> compare_with_tolerance(const Left& left, const Right& right)
        {
            auto res = left == right;
            if (!res)
            {
                std::stringstream out;
                out << '[' << left << " != " << right << ']';
                return std::make_tuple(res, out.str());
            }
            return std::make_tuple(true, std::string());
        }

        enum operation
        {
            eq, neq, gt, lt, ge, le
        };

        template<operation op>
        struct calculus;

        _CALCULUS_(eq, == , " != ");
        _CALCULUS_(neq, != , " == ");
        _CALCULUS_(gt, > , " <= ");
        _CALCULUS_(lt, < , " >= ");
        _CALCULUS_(ge, >= , " > ");
        _CALCULUS_(le, <= , " < ");

        template<typename Left, typename Right, operation op, typename _ = void>
        struct operations;

        template<typename Left, typename Right, operation op>
        struct operations<Left, Right, op, std::enable_if_t<traits_double<Left, Right>::is_arithmetic || traits_double<Left, Right>::is_string || traits_double<Left, Right>::is_boolean>>
        {

            static std::tuple<bool, std::string> apply(const Left &left, const Right &right)
            {
                if (tolerance() == 0.)
                {
                    auto res = calculus<op>::apply(left, right);
                    if (!res)
                    {
                        std::stringstream out;
                        out << '[' << left << calculus<op>::inverse() << right << ']';
                        return std::make_tuple(res, out.str());
                    }
                    return std::make_tuple(res, std::string());
                }
                return compare_with_tolerance(left, right);
            }

        };

        template<typename T>
        void reverse(std::stack<T> &in)
        {
            std::stack<T> out;
            while (!in.empty())
            {
                out.push(std::move(in.top()));
                in.pop();
            }
            in = std::move(out);
        }

        template<typename T>
        std::string to_string(const T &t)
        {
            std::stringstream out;
            out << t;
            return out.str();
        }

        template<typename Left, typename Right, operation op>
        struct operations<Left, Right, op, std::enable_if_t<traits_double<Left, Right>::is_container>>
        {

            using value_left =typename detail::traits<Left>::value_type;
            using value_right = typename detail::traits<Right>::value_type;

            static std::tuple<bool, std::string> apply(const Left &left, const Right &right)
            {
                if (left.size() == right.size())
                {
                    if constexpr (traits_double<value_left, value_right>::is_container)
                    {
                        std::stack<std::string> path;
                        std::size_t i = 0;
                        for (auto v : sl::tools::zip(left, right))
                        {
                            if constexpr (traits_double<Left, Right>::is_key_container)
                            {
                                path.emplace(to_string(std::get<0>(v).first));
                                auto res = operations<typename Left::key_type, typename Right::key_type, eq>::apply(std::get<0>(v).first, std::get<1>(v).first);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    reverse(path);
                                    out << "in the position ";
                                    while (!path.empty())
                                    {
                                        out << '[' << path.top() << ']';
                                        path.pop();
                                    }
                                    out << " with key error " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                                else
                                {
                                    res = operations<value_left, value_right, op>::apply(std::get<0>(v).second, std::get<1>(v).second, path);
                                    if (!std::get<0>(res))
                                    {
                                        std::stringstream out;
                                        reverse(path);
                                        out << "in the position ";
                                        while (!path.empty())
                                        {
                                            out << '[' << path.top() << ']';
                                            path.pop();
                                        }
                                        out << " with error " << std::get<1>(res);
                                        return std::make_tuple(false, out.str());
                                    }
                                }
                                path.pop();
                            }
                            else
                            {
                                path.emplace(std::to_string(i));
                                auto res = operations<value_left, value_right, op>::apply(std::get<0>(v), std::get<1>(v), path);
                                if (std::get<0>(res))
                                {
                                    path.pop();
                                }
                                else
                                {
                                    std::stringstream out;
                                    reverse(path);
                                    out << "in the position ";
                                    while (!path.empty())
                                    {
                                        out << '[' << path.top() << ']';
                                        path.pop();
                                    }
                                    out << " with error " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                            }
                            i++;
                        }
                        return std::make_tuple(true, std::string());
                    }
                    else
                    {
                        std::size_t i = 0;
                        for (auto v: sl::tools::zip(left, right))
                        {
                            if constexpr (traits_double<Left, Right>::is_key_container)
                            {
                                auto res = operations<typename Left::key_type, typename Right::key_type, eq>::apply(std::get<0>(v).first, std::get<1>(v).first);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " problem with the keys " << std::get<1>(res) << " in the position " << i;
                                    return std::make_tuple(false, out.str());
                                }
                                res = operations<value_left, value_right, op>::apply(std::get<0>(v).second, std::get<1>(v).second);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " in the position " << i << " with key value " << std::get<0>(v).first << " with values " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                            }
                            else
                            {
                                auto res = operations<value_left, value_right, op>::apply(std::get<0>(v), std::get<1>(v));
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " in the element " << i << " with values " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                            }
                            i++;
                        }
                    }
                    return std::make_tuple(true, std::string());
                }
                return std::make_tuple(false, std::string("incorrect size"));
            }

            static std::tuple<bool, std::string> apply(const Left& left, const Right& right, std::stack<std::string>& path)
            {
                if (left.size() == right.size())
                {
                    if constexpr (traits_double<value_left, value_right>::is_container)
                    {
                        std::size_t i = 0;
                        for (auto v : sl::tools::zip(left, right))
                        {
                            if constexpr (traits_double<Left, Right>::is_key_container)
                            {
                                path.emplace(to_string(std::get<0>(v).first));
                                auto res = operations<typename Left::key_type, typename Right::key_type, eq>::apply(std::get<0>(v).first, std::get<1>(v).first);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " with key error " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                                else
                                {
                                    res = operations<value_left, value_right, op>::apply(std::get<0>(v).second, std::get<1>(v).second, path);
                                    if (!std::get<0>(res))
                                    {
                                        return res;
                                    }
                                }
                                path.pop();
                            }
                            else
                            {
                                path.emplace(std::to_string(i));
                                auto res = operations<value_left, value_right, op>::apply(std::get<0>(v), std::get<1>(v), path);
                                if (!std::get<0>(res))
                                {
                                    return res;
                                }
                                path.pop();
                            }
                            i++;
                        }
                        return std::make_tuple(true, std::string());
                    }
                    else
                    {
                        std::size_t i = 0;
                        for (auto v : sl::tools::zip(left, right))
                        {
                            if constexpr (traits_double<Left, Right>::is_key_container)
                            {
                                path.emplace(to_string(std::get<0>(v).first));
                                auto res = operations<typename Left::key_type, typename Right::key_type, eq>::apply(std::get<0>(v).first, std::get<1>(v).first);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " problem with the keys in the position " << i;
                                    return std::make_tuple(false, out.str());
                                }
                                res = operations<value_left, value_right, op>::apply(std::get<0>(v).second, std::get<1>(v).second);
                                if (!std::get<0>(res))
                                {
                                    std::stringstream out;
                                    out << " with values " << std::get<1>(res);
                                    return std::make_tuple(false, out.str());
                                }
                                path.pop();
                            }
                            else
                            {
                                path.emplace(std::to_string(i));
                                auto res = operations<value_left, value_right, op>::apply(std::get<0>(v), std::get<1>(v));
                                if (!std::get<0>(res))
                                {
                                    return res;;
                                }
                                path.pop();
                            }
                            i++;
                        }
                    }
                    return std::make_tuple(true, std::string());
                }
                return std::make_tuple(false, std::string("incorrect size"));
            }
        };

        template<typename Left, typename Right, operation op>
        struct operations<Left, Right, op, std::enable_if_t<traits_double<Left, Right>::is_tuple>>
        {
            static std::tuple<bool, std::string> apply(const Left& left, const Right& right)
            {
                if constexpr (std::tuple_size_v<Left> == std::tuple_size_v<Right>)
                {
                    const auto size = std::tuple_size_v<Left>;
                    std::tuple<bool, std::string> output = std::make_tuple(true, std::string());
                    sl::tools::for_each(std::make_index_sequence<size>{}, [&](auto i)
                    {
                        if (std::get<0>(output))
                        {
                            auto res = operations<std::tuple_element_t<i, Left>, std::tuple_element_t<i, Right>, op>::apply(std::get<i>(left), std::get<i>(right));
                            if (!std::get<0>(res))
                            {
                                std::stringstream out;
                                out << "Problem in the element " << i << " of the tuple, with error " << std::get<1>(res);
                                output = std::make_tuple(false, out.str());
                            }
                        }
                    });
                    return output;
                }
                return std::make_tuple(false, std::string("incorrect size"));
            }
        };

        template<typename Left, typename Right, operation op>
        struct operations<Left, Right, op, std::enable_if_t<traits_double<Left, Right>::is_optional>>
        {
            static std::tuple<bool, std::string> apply(const Left& left, const Right& right)
            {

                using left_type = typename Left::value_type;
                using right_type = typename Right::value_type;

                if (left.has_value() && left.has_value())
                {
                    return operations<left_type, right_type, op>::apply(left.value(), right.value());
                }
                else if (!left.has_value() && left.has_value() || left.has_value() && !left.has_value())
                {
                    return std::make_tuple(false, std::string("both need to be in the same state"));
                }
                return std::make_tuple(true, std::string());
            }

        };

        template<typename T>
        struct variable;

        struct builder
        {
            ~builder() = default;
            builder() = default;

            template<typename T>
            variable<std::decay_t<T>> operator->*(T&& t)
            {
                return variable<std::decay_t<T>>(std::forward<T>(t));
            }

        };

        template<typename T>
        struct variable
        {
            using type = typename std::decay_t<T>;
            ~variable() = default;
            variable() = delete;
            template<typename U>
            variable(U&& u) : value_(std::forward<U>(u)) {}

            std::tuple<bool, std::string> validate() const
            {
                return std::make_tuple(static_cast<bool>(value_), std::string());
            }

            T value_;
        };

        template<typename Left, typename Right, operation op>
        struct expr
        {

            using left = typename Left::type;
            using right = std::decay_t<Right>;

            ~expr() = default;
            expr() = delete;
            template<typename L, typename R>
            expr(L&& l, R&& r) : left_(std::forward<L>(l)), right_(std::forward<R>(r)) {}

            std::tuple<bool, std::string> validate() const
            {
                return operations<left, right, op>::apply(left_.value_, right_);
            }

            Left left_;
            Right right_;
        };

        _OPERATORS_EXPR_(==, operation::eq)
        _OPERATORS_EXPR_(!=, operation::neq)
        _OPERATORS_EXPR_(>, operation::gt)
        _OPERATORS_EXPR_(< , operation::lt)
        _OPERATORS_EXPR_(>= , operation::ge)
        _OPERATORS_EXPR_(<= , operation::le)

        _OPERATORS_(+)
        _OPERATORS_(-)
        _OPERATORS_(*)
        _OPERATORS_(/)

    } // namespace test

} // namespace sl

#undef _OPERATORS_
#undef _OPERATORS_EXPR_
#undef _CALCULUS_

#define SL_TEST_BUILD_EXPR(P) (sl::test::builder()->*P)

#define SL_TEST_WITHOUT_PRECISSION(x) {auto t = SL_TEST_BUILD_EXPR(x); auto res = t.validate(); if(!std::get<0>(res)){\
                                                message_error::instance().add_file(__FILE__); message_error::instance().add_line(__LINE__); message_error::instance().add_operation(#x);\
                                                auto& res_message = std::get<1>(res); message_error::instance().add_message(std::move(res_message)); message_error::instance().print_last();}}

#define SL_TEST_WITH_PRECISSION(x, y) {auto old_precision = sl::test::tolerance(); sl::test::tolerance() = y; auto t = SL_TEST_BUILD_EXPR(x); auto res = t.validate(); if(!std::get<0>(res)){\
                                                message_error::instance().add_file(__FILE__); message_error::instance().add_line(__LINE__); message_error::instance().add_operation(#x);\
                                                auto& res_message = std::get<1>(res); message_error::instance().add_message(std::move(res_message)); message_error::instance().print_last();}sl::test::tolerance() = old_precision;}

#define SL_INVOKE_BY_NUMBER_ARGS(N, F1, F2, ...) SL_IIF(SL_EQUAL(SL_NARG(__VA_ARGS__), N))(F1, F2)(__VA_ARGS__)

#define SL_TEST(...) SL_INVOKE_BY_NUMBER_ARGS(2, SL_TEST_WITH_PRECISSION, SL_TEST_WITHOUT_PRECISSION, __VA_ARGS__)
