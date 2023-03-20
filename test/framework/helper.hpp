#pragma once

#include <sl/macros/IIF.h>
#include <sl/macros/NARG.h>
#include <sl/macros/EQUAL.h>
#include <type_traits>

namespace SL
{
    namespace test
    {

        inline double& tolerance()
        {
            static double eps = 0.;
            return eps;
        }

        template<typename Left, typename Right>
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

        enum operation
        {
            equal
        };

        template<typename Left, typename Right, operation op, typename _ = void>
        struct operations;

        template<typename Left, typename Right>
        struct operations<Left, Right, equal, std::enable_if_t<std::is_arithmetic_v<Left>&& std::is_arithmetic_v<Right>>>
        {

            static std::tuple<bool, std::string> apply(const Left& left, const Right& right)
            {
                if (tolerance() == 0.)
                {
                    return std::make_tuple(left == right, std::string());
                }
                return compare_with_tolerance(left, right);
            }

            static std::string report(const Left& left, const Right& right)
            {
                std::string res;
                res += '[';
                res += std::to_string(left);
                res += std::string(" != ");
                res += std::to_string(right);
                res += ']';
                return res;
            }
        };



        template<typename Left, typename Right>
        struct operations<Left, Right, equal, std::enable_if_t<!std::is_arithmetic_v<Left> || !std::is_arithmetic_v<Right>>>
        {

            static std::tuple<bool, std::string> apply(const Left& left, const Right& right)
            {
                return std::make_tuple(left == right, std::string());
            }

            static std::string report(const Left& left, const Right& right)
            {
                std::string res;
                res += '[';
                res += left;
                res += std::string(" != ");
                res += right;
                res += ']';
                return res;
            }
        };


        template<typename T>
        struct variable;

        struct builder
        {
            ~builder() = default;
            builder() = default;

            template<typename T>
            variable<T> operator->*(T&& t)
            {
                return variable<T>(std::forward<T>(t));
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

            std::string report() const
            {
                return "";
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

            std::string report() const
            {
                return operations<left, right, op>::report(left_.value_, right_);
            }

            Left left_;
            Right right_;
        };

        template<typename Left, typename Right>
        expr<variable<Left>, Right, operation::equal> operator==(const variable<Left>& l, Right&& r)
        {
            return expr<variable<Left>, Right, operation::equal>(l, std::forward<Right>(r));
        }
    }
}

#define SL_TEST_BUILD_EXPR(P) (SL::test::builder()->*P)

#define SL_TEST_WITHOUT_PRECISSION(x) {auto t = SL_TEST_BUILD_EXPR(x); auto res = t.validate(); if(!std::get<0>(res)){\
                                                message_error::Instance().add_file(__FILE__); message_error::Instance().add_line(__LINE__); message_error::Instance().add_operation(#x);\
                                                std::string repport = t.report(); const auto& res_message = std::get<1>(res); \
                                                if (res_message.empty()){message_error::Instance().add_message(std::move(repport));}else{message_error::Instance().add_message(repport + ": " + std::get<1>(res));} message_error::Instance().print_last();}}

#define SL_TEST_WITH_PRECISSION(x, y) {auto old_precision = SL::test::tolerance(); SL::test::tolerance() = y; auto t = SL_TEST_BUILD_EXPR(x); auto res = t.validate(); if(!std::get<0>(res)){\
                                                message_error::Instance().add_file(__FILE__); message_error::Instance().add_line(__LINE__); message_error::Instance().add_operation(#x);\
                                                std::string repport = t.report(); const auto& res_message = std::get<1>(res); \
                                                if (res_message.empty()){message_error::Instance().add_message(std::move(repport));}else{message_error::Instance().add_message(repport + ": " + std::get<1>(res));} message_error::Instance().print_last();}SL::test::tolerance() = old_precision; }

#define SL_INVOKE_BY_NUMBER_ARGS(N, F1, F2, ...) SL_IIF(SL_EQUAL(SL_NARG(__VA_ARGS__), N))(F1, F2)(__VA_ARGS__)

#define SL_TEST(...) SL_INVOKE_BY_NUMBER_ARGS(2, SL_TEST_WITH_PRECISSION, SL_TEST_WITHOUT_PRECISSION, __VA_ARGS__)

