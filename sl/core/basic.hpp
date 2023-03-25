#pragma once

#include <sl/patterns/singleton.h>

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

		template<typename IN, typename OUT, typename A>
		struct builder_base
		{
			virtual ~builder_base() = default;
			builder_base() = default;

			virtual bool test(const A *a) const = 0;
			virtual void build(int version, IN &in, A *&output) = 0;
			virtual void build(int version, IN &in, std::unique_ptr<A> &output) = 0;
			virtual void build(int version, IN &in, std::shared_ptr<A> &output) = 0;
			virtual void build(IN &in, A *&) const = 0;
			virtual void build(IN &in, std::unique_ptr<A> &output) = 0;
			virtual void build(IN &in, std::shared_ptr<A> &output) = 0;
			virtual void save(A *a, OUT &out) const = 0;
			virtual const char *getClassName() const = 0;
			virtual size_t getVersion() const = 0;
		};

		template<typename IN, typename OUT, typename A, typename B>
		struct builder_derivated : builder_base<IN, OUT, A>
		{
			using base = std::decay_t<A>;
			using derived = std::decay_t<B>;

			virtual ~builder_derivated() = default;
			builder_derivated() = default;

			template<typename B>
			size_t get_type(const std::shared_ptr<B> &b)
			{
				return get_type(b.get());
			}

			template<typename B>
			size_t get_type(const std::unique_ptr<B> &b)
			{
				return get_type(b.get());
			}

			template<typename B>
			size_t get_type(B *b)
			{
				size_t i = 0;
				for(const auto &v : builders_)
				{
					if(v->test(b))
					{
						return i;
					}
					i++;
				}
				// The link between the base and the derived class don't exist
				throw std::bad_cast();
			}

			template<typename B>
			void add(B *b)
			{
				bool is_found = false;
				for(const auto &v : builders_)
				{
					if(v->test(b))
					{
						is_found = true;
						break;
					}
				}
				if(!is_found)
				{
					builders_.emplace_back(new builder_derivated<IN, OUT, A, B>());
				}
			}

			template<typename T>
			void build(size_t i, IN &in, T &ptr)
			{
				if(i < builders_.size())
				{
					return builders_[i]->build(in, ptr);
				}
				throw std::bad_alloc();
			}

			template<typename T>
			void build(const std::string &name, int version, IN &in, T &ptr)
			{
				auto it = position_.find(name);
				if(it != position_.end())
				{
					return builders_[it->second]->build(version, in, ptr);
				}
				else
				{
					size_t i = 0;
					for(const auto &v : builders_)
					{
						const char * base_name = v->getClassName();
						if(!strcmp(base_name, name.c_str()))
						{
							position_.emplace(std::make_pair(name, i));
							return v->build(version, in, ptr);
						}
						i++;
					}
				}
				throw std::bad_alloc();
			}

			void save(size_t i, A *&t, OUT &out)
			{
				if(i < builders_.size())
				{
					return builders_[i]->save(t, out);
				}
				throw std::bad_cast();
			}

			const char * getClassName(size_t i) const
			{
				if(i < builders_.size())
				{
					return builders_[i]->getClassName();
				}
				throw std::bad_cast();
			}

			size_t getVersion(size_t i) const
			{
				if(i < builders_.size())
				{
					return builders_[i]->getVersion();
				}
				throw std::bad_cast();
			}

		protected:

			std::vector<builder_base<IN, OUT, A> *> builders_;
			std::unordered_map<std::string, size_t> position_;

		};

		template<typename IN, typename OUT, typename T, typename U>
		void link_derived(const T *t = nullptr, const U *u = nullptr)
		{
			sl::singleton<detail::builder_derivated<IN, OUT, T, U>>::instance().add(u);
		}

		template<typename IN, typename OUT, typename T, typename std::enable_if_t<!std::is_abstract<T>::value, int> = 0>
		void link_derived(const T *t = nullptr)
		{
			sl::singleton<detail::builder_derivated<IN, OUT, T, T>>::instance().add(t);
		}

		template<typename IN, typename OUT, typename T, typename std::enable_if_t<std::is_abstract<T>::value, int> = 0>
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
			static void create(T &obj)
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
			static void create(T &obj)
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

	template<typename T, typename U>
	constexpr auto ptr_array(T &&t, U &&u)
	{
		return detail::ptr_array<T, U>(std::forward<T>(t), std::forward<U>(u));
	}

	template<typename C, typename T, typename U>
	constexpr auto array_property(T C::*member1, U C::*member2, const char *name)
	{
		return detail::array_property<C, T, U, detail::simple_allocator>(member1, member2, name);
	}

	template<typename A>
	struct  builder_with_allocator
	{
		template<typename C, typename T, typename U>
		constexpr auto array_property(T C::*member1, U C::*member2, const char *name)
		{
			return detail::array_property<C, T, U, A>(member1, member2, name);
		}
	};
	
    
} // namespace gt
