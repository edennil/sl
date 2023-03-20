#pragma once

namespace sl
{

    namespace patterns
    {

        namespace detail
        {

            template<typename T>
            struct build_with_new
            {

                static T * build()
                {
                    return new T;
                }

                static void destroy(T *&t)
                {
                    delete t, t = nullptr;
                }

            };

        } // namespace detail

        template<typename T, typename builder = detail::build_with_new<T>>
        class singleton
        {
        public:
            static T & instance() 
            {
                if (instance_ == nullptr)
                {
                    instance_ = builder::build();
                }
                return *instance_;
            }
        protected:

            singleton() = delete;
            singleton(const singleton &) = delete;
            singleton(singleton &&) = delete;
            singleton& operator=(const singleton &) = delete;
            singleton& operator=(singleton &&) = delete;

            static T * instance_;
        };

        template<typename T, typename builder> T * singleton<T, builder>::instance_ = nullptr;

    } // namespace patterns

} // namespace sl