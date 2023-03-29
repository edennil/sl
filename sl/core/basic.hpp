#pragma once

#include <type_traits>
#include <sl/patterns/singleton.hpp>

namespace sl
{
        
    namespace detail
    {
        template <typename T, int N>
        constexpr int dim(T(&)[N])
        {
            return N;
        }

        template<typename T>
        struct witout_head
        {
            unsigned int version_;
            T* ptr_;
        };

        template<typename T>
        struct class_data;

        template<typename T>
        struct class_data
        {
            constexpr static unsigned int version = 0;
        };

        template<typename A>
        struct builder;

        namespace impl
        {
            template<typename A>
            struct base_class
            {
                virtual ~base_class() = default;
                base_class() = default;
                virtual bool test(const A*) const = 0;
                virtual bool test(const std::string &) const = 0;
                virtual const std::string &name(const A*) const = 0;
                virtual unsigned int version(const A*) const = 0;
                virtual void save(const A* a, json_oarchive &out) const = 0;
                virtual bool build(const std::string& name, int version, json_iarchive& in, A*&ptr) = 0;
            };

            template<typename A, typename B>
            struct derived_class : public base_class<A>
            {
                virtual ~derived_class() = default;
                derived_class() = default;

                bool test(const A *a) const final
                {
                    const B* b = dynamic_cast<const B*>(a);
                    if (b)
                    {
                        return true;
                    }
                    return false;
                }

                bool test(const std::string &name) const final
                {
                    return sl::patterns::singleton<sl::detail::builder<B>>::instance().test(name);
                }

                const std::string &name(const A* a) const final
                {
                    if constexpr (std::is_polymorphic_v<B>)
                    {
                        return sl::patterns::singleton<sl::detail::builder<B>>::instance().name(static_cast<const B *>(a));
                    }
                    return std::string(class_data<B>::name);
                }

                unsigned int version(const A* a) const final
                {
                    if constexpr (std::is_polymorphic_v<B>)
                    {
                        return sl::patterns::singleton<sl::detail::builder<B>>::instance().version(static_cast<const B*>(a));
                    }
                    return class_data<B>::version;
                }

                void save(const A* a, json_oarchive& out) const final
                {
                    if constexpr (std::is_polymorphic_v<B>)
                    {
                        return sl::patterns::singleton<sl::detail::builder<B>>::instance().save(static_cast<const B*>(a), out);
                    }
                    helper<B>::serialize(out, *a);
                }

                bool build(const std::string& name, int version, json_iarchive& in, A *&a)
                {
                    if constexpr (std::is_polymorphic_v<B>)
                    {
                        if (sl::patterns::singleton<sl::detail::builder<B>>::instance().test(name))
                        {
                            B* b = nullptr;
                            sl::patterns::singleton<sl::detail::builder<B>>::instance().build(name, version, in, b);
                            a = b;
                            return true;
                        }
                    }
                    return false;
                }

            };
        }

        template<typename A>
        struct builder
        {

            virtual ~builder() = default;
            builder()
            {
                name_ = class_data<A>::name;
            }

            template<typename C>
            void add(const C* c)
            {
                std::string name(class_data<C>::name);
                if constexpr (std::is_same_v<A, C>)
                {
                    if (base_ == nullptr)
                    {
                        base_ = new impl::derived_class<A, C>();
                    }
                }
                else
                {
                    auto it = position_.find(name);
                    if (it == position_.end())
                    {
                        position_.emplace(std::move(name), builders_.size());
                        builders_.emplace_back(new impl::derived_class<A, C>());
                    }
                }
            }

            const std::string &name(const A* c) const
            {
                for (const auto& v : builders_)
                {
                    if (v->test(c))
                    {
                        return v->name(c);
                    }
                }
                return name_;
            }

            bool test(const std::string& name)
            {
                bool res = name_ == name;
                for (const auto& v : builders_)
                {
                    res |= v->test(name);
                }
                return res;
            }

            unsigned int version(const A* c) const
            {
                for (const auto& v : builders_)
                {
                    if (v->test(c))
                    {
                        return v->version(c);
                    }
                }
                return class_data<A>::version;
            }

            template<typename OUT>
            void save(const A* a, OUT& out) const
            {
                for (const auto& v : builders_)
                {
                    if (v->test(a))
                    {
                        return v->save(a, out);
                    }
                }
                access::serialize(out, *a, class_data<A>::version);
            }

            template<typename IN, typename T>
            void build(const std::string& name, int version, IN& in, T *&ptr)
            {
                if (name == name_)
                {
                    ptr_wrapper<T *>::create(ptr);
                    return access::deserialize(in, *ptr, version);
                }
                for (const auto& v : builders_)
                {
                    if (v->build(name, version, in, ptr))
                    {
                        return;
                    }
                }
                throw std::bad_typeid();
            }

            impl::base_class<A>* base_ = nullptr;
            std::vector<impl::base_class<A> *> builders_;
            std::unordered_map<std::string, size_t> position_;

            std::string name_;
        };

        template<typename T, typename U>
        void link_derived(const T *t = nullptr, const U *u = nullptr)
        {
            sl::patterns::singleton<detail::builder<T>>::instance().add(u);
        }

        template<typename T, typename std::enable_if_t<!std::is_abstract<T>::value, int> = 0>
        void link_derived(const T *t = nullptr)
        {
            sl::patterns::singleton<detail::builder<T>>::instance().add(t);
        }

        template<typename T, typename std::enable_if_t<std::is_abstract<T>::value, int> = 0>
        void link_derived(const T *t = nullptr)
        {
        }

        template<typename T, T...ARCHIVE, typename F>
        constexpr void for_each(std::integer_sequence<T, ARCHIVE...>, F &&f)
        {
            using tmp = int [];
            (void)tmp{
                (static_cast<void>(f(std::integral_constant<T, ARCHIVE>{})), 0)..., 0
            };
        }

        template<typename C, typename T, bool O>
        struct basic_property
        {
            constexpr basic_property(T C::*some_member, const char *name) : member_{ some_member}, name_{name} {}
            using type = T;
            T C::*member_;
            const char *name_;
            constexpr static bool optional_{false};
        };

        struct simple_allocator
        {
            template<typename T>
            static T * allocate(size_t size)
            {
                return new T[size];
            }
        };

        template<typename C, typename T, typename U, typename A>
        struct array_property
        {
            using allocator = A;
            constexpr array_property(T C::*some_member1, U C::*some_member2, const char *name) : member1_{some_member1}, member2_{some_member2}, name_{name} {}
            using type = U;
            T C::*member1_;
            U C::*member2_;
            const char *name_;
            constexpr static bool optional_{false};
        };

        template<typename T>
        struct ptr_wrapper
        {
            using type = typename T::element_type;
            static type *get(const T &obj)
            {
                return obj.get();
            }

            template<typename U>
            static void create(T &obj)
            {
                obj = T(new type);
            }
        };

        template<typename T>
        struct ptr_wrapper<T *>
        {

            using type = T;
            static type *get(T *obj)
            {
                return obj;
            }

            template<typename U>
            static void create(U &obj)
            {
                obj = new type;
            }
        };

        template<typename T>
        struct ptr_wrapper<std::shared_ptr<T>>
        {
            using type = T;
            static type *get(const std::shared_ptr<T> &obj)
            {
                return obj.get();
            }

            template<typename U>
            static void create(U &obj)
            {
                obj = std::make_shared<type>();
            } 
        };

        template<typename A>
        struct ptr_array
        {

            template<typename SIZE, typename T>
            static void create(SIZE size, T *&obj)
            {
                obj = A::template allocate<T>(size);
            }
        };

    } // namespace detail
    
    template<typename T, typename U>
    const T & cast_base(const U &u)
    {
        return static_cast<const T &>(u);
    }

    template<typename T, typename U>
    const T * cast_base(const U *u)
    {
        return static_cast<const T *>(u);
    }

    template<typename T, typename U>
    T & cast_base(U &u)
    {
        return static_cast<T &>(u);
    }
    
    template<typename T, typename U>
    T * cast_base(U *u)
    {
        return static_cast<T *>(u);
    }

    template<typename C, typename T>
    constexpr auto basic_property(T C::*member, const char *name)
    {
        return detail::basic_property<C, T, false>(member, name);
    }

    template<typename C, typename T>
    constexpr auto optional_basic_property(T C::*member, const char *name)
    {
        return detail::basic_property<C, T, true>(member, name);
    }

    template<typename C, typename T, typename U>
    constexpr auto array_property(T C::*member1, U C::*member2, const char *name)
    {
        return detail::array_property<C, T, U, detail::simple_allocator>(member1, member2, name);
    }

    template<typename A>
    struct builder_with_allocator
    {
        template<typename C, typename T, typename U>
        constexpr auto array_property(T C::*member1, U C::*member2, const char *name)
        {
            return detail::array_property<C, T, U, A>(member1, member2, name);
        }
    };
    
    
} // namespace sl
