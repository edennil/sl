#pragma once

#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <string>

#include <sl/patterns/singleton.hpp>

namespace sl
{
    namespace test
    {
        namespace detail
        {
            class message_error;
        } // namespace detail

    } // namespace test

} // namespace sl

using message_error = sl::patterns::singleton<sl::test::detail::message_error>;

namespace sl
{

    namespace test
    {

        namespace detail
        {
            class message_error
            {
            public:
                void add_current_test(const std::string &group, const std::string &test)
                {
                    current_group_ = group;
                    current_test_ = test;
                }
                void add_file(std::string &&file) { file_ = std::move(file); }
                void add_line(int line) { line_ = line; }
                void add_operation(std::string &&operation) { operation_ = std::move(operation); }
                void add_message(std::string &&message) { message_ = std::move(message); }

                const std::string &get_file(std::string &&file) const { return file_; }
                int get_line(int line) const { return line_; }
                const std::string &get_operation(std::string &&operation) const { return operation_; }
                const std::string &get_message(std::string &&message) const { return message_; }

                void print_last()
                {
                    auto to_print = build_message();
                    error_ = true;
                    clean();
                    if (!std::get<0>(to_print).empty())
                    {
                        auto tmp_message = std::get<0>(to_print) + std::get<1>(to_print);
                        if (messages_.find(current_group_) == messages_.end())
                        {
                            messages_[current_group_] = std::map<std::string, std::vector<std::string>>();
                        }
                        messages_[current_group_][current_test_].push_back(std::move(tmp_message));
                        printf("%s\x1B[91m%s\033[0m\n", std::get<0>(to_print).c_str(), std::get<1>(to_print).c_str());
                    }
                }

                void clean_report()
                {
                    current_group_.clear();
                    current_test_.clear();
                    messages_.clear();
                }

                void print_report()
                {
                    std::size_t big_total = 0;
                    if (!messages_.empty())
                    {
                        for (const auto &m : messages_)
                        {
                            std::size_t total = 0;
                            for (const auto &n : m.second)
                            {
                                if (!n.second.empty())
                                {
                                    auto partial = n.second.size();
                                    total += partial;
                                }
                            }
                            if (total == 0)
                            {
                                printf("\x1B[92m *** No failures detected\033[0m\n");
                            }
                            else
                            {
                                big_total += total;
                                printf("\x1B[91m%d failures are detected in the group %s\033[0m\n", static_cast<int>(total), m.first.c_str());
                            }
                        }
                    }
                    if (big_total == 0)
                    {
                        printf("\x1B[92m *** No failures detected\033[0m\n");
                    }
                    else
                    {
                        printf("\x1B[91m%d failures are detected in total\033[0m\n", static_cast<int>(big_total));
                    }
                }

                bool error() const
                {
                    return error_;
                }

            protected:

                void clean()
                {
                    file_.clear();
                    line_ = -1;
                    operation_.clear();
                    message_.clear();
                }

                std::tuple<std::string, std::string> build_message() const
                {
                    if (!file_.empty())
                    {
                        std::string file_message = file_ + "(" + std::to_string(line_) + "):";
                        std::string error = " error: in \"" + current_group_ + '/' + current_test_ + "\": check " + operation_ + " has failed";
                        if (!message_.empty())
                        {
                            error += ' ' + message_;
                        }
                        return std::make_tuple(std::move(file_message), std::move(error));
                    }
                    return std::make_tuple(std::string(), std::string());
                }

                std::string current_group_;
                std::string current_test_;
                std::map<std::string, std::map<std::string, std::vector<std::string>>> messages_;

                std::string file_;
                int line_ = -1;
                std::string operation_;
                std::string message_;
                bool error_ = false;
            };
        }

        class tester
        {
        public:
            using functor = std::function<void(void)>;
            ~tester() = default;
            tester() = default;
            void insert(const char *name, const functor &func)
            {
                std::string tmpname(name);
                std::string::size_type i = tmpname.find("::");
                std::string name_(tmpname), group_("OTHER");
                if (i != std::string::npos)
                {
                    group_ = tmpname.substr(0, i);
                    name_ = tmpname.substr(i + 2, tmpname.size());
                }
                auto iter = functors_.find(group_);
                auto end = functors_.end();
                if (iter == end)
                {
                    functors tmp;
                    tmp.emplace(std::make_pair(std::move(name_), func));
                    functors_.emplace(std::make_pair(std::move(group_), std::move(tmp)));
                }
                else
                {
                    auto iter_ = iter->second.find(name_);
                    auto end_ = iter->second.end();
                    if (iter_ != end_)
                    {
                        throw name_.c_str();
                    }
                    iter->second.emplace(std::move(name_), func);
                }
            }
            void insert(const char *name, const char *group, const functor &func)
            {
                std::string name_(name), group_(group);
                auto iter = functors_.find(group_);
                auto end = functors_.end();
                if (iter == end)
                {
                    functors tmp;
                    tmp.emplace(std::move(name_), func);
                    functors_.emplace(std::move(group_), std::move(tmp));
                }
                else
                {
                    auto iter_ = iter->second.find(name_);
                    auto end_ = iter->second.end();
                    if (iter_ != end_)
                    {
                        throw name_.c_str();
                    }
                    iter->second.emplace(std::move(name_), func);
                }
            }
            void run(void) const
            {
                for (const auto &n : functors_)
                {
                    for (const auto &m : n.second)
                    {
                        message_error::instance().add_current_test(n.first, m.first);
                        std::cout << "Running " << m.first << std::endl;
                        m.second();
                        message_error::instance().print_report();
                        message_error::instance().clean_report();
                    }
                }
            }
            void run(const char *group) const
            {
                auto iter = functors_.find(group);
                auto end = functors_.end();
                if (iter != end)
                {
                    for (const auto &m : iter->second)
                    {
                        message_error::instance().add_current_test(group, m.first);
                        std::cout << "Running " << m.first << std::endl;
                        m.second();
                        message_error::instance().print_report();
                        message_error::instance().clean_report();
                    }
                }
            }
            void run(const char *group, const char *name) const
            {
                auto iter = functors_.find(group);
                auto end = functors_.end();
                if (iter != end)
                {
                    auto iter_ = iter->second.find(name);
                    auto end_ = iter->second.end();
                    if (iter_ != end_)
                    {
                        message_error::instance().add_current_test(group, name);
                        std::cout << "Running " << name << std::endl;
                        iter_->second();
                        message_error::instance().print_report();
                        message_error::instance().clean_report();
                    }
                    else
                    {
                        throw name;
                    }
                }
            }

            bool error()
            {
                return message_error::instance().error();
            }

        protected:

            using functors = std::map<std::string, functor>;
            using container = std::map<std::string, functors>;

            container functors_;

        };

    }

}

using tester = sl::patterns::singleton<sl::test::tester>;

#define SL_ADD_TEST(A) tester::instance().insert(#A, A)
#define SL_RUN_TEST(A)                                         \
    {                                                          \
        std::string tmpname(#A);                               \
        std::string::size_type i = tmpname.find("::");         \
        std::string name_(tmpname), group_("OTHER");           \
        if (i != std::string::npos)                            \
        {                                                      \
            group_ = tmpname.substr(0, i);                     \
            name_ = tmpname.substr(i + 2, tmpname.size());     \
        }                                                      \
        tester::instance().run(group_.c_str(), name_.c_str()); \
    }
#define SL_RUN_GROUP(A) tester::instance().run(#A)

#define SL_ADD_TEST_CASE(A)                                           \
    void A(void);                                                     \
    namespace                                                         \
    {                                                                 \
        struct Initializer##A                                         \
        {                                                             \
            Initializer##A()                                          \
            {                                                         \
                tester::instance().insert(#A, group_name.c_str(), A); \
            }                                                         \
        };                                                            \
        Initializer##A initializer##A;                                \
    }                                                                 \
    void A(void)

#define SL_TEST_GROUP(A)   \
    namespace A            \
    {                      \
        std::string group_name(#A);

#define SL_END_TEST_GROUP }
