#pragma once

namespace sl
{

    namespace detail
    {

        namespace json
        {

            namespace fundamenta_tools
            {

                template<typename T>
                struct helper
                {
                    static void serialize(json_oarchive& out, T obj)
                    {
                        out.get() << obj;
                    }
                };

                template<typename T>
                struct helper_in_vector
                {
                    static void deserialize(json_iarchive& in, T& obj)
                    {
                        auto p = in.find_first_of(',');
                        auto q = in.find_first_of(']');
                        p = p < q ? p : q;
                        auto size = p - in.position();
                        obj = sl::detail::to_value<std::decay_t<T>>(in.substr(size).c_str());
                        in += size;
                    }
                };

                template<>
                struct helper_in_vector<std::string>
                {
                    static void deserialize(json_iarchive& in, std::string& obj)
                    {
                        in++;
                        auto p = in.find_first_of('\"');
                        auto size = p - in.position();
                        obj = in.substr(size);
                        in += static_cast<int>(size + 1);
                    }
                };

                //template<>
                //struct helper_in_map<std::string>
                //{
                //    static void deserialize(json_iarchive& in, std::string& obj)
                //    {
                //        in++;
                //        auto p = in.find_first_of('\"');
                //        auto size = p - in.position();
                //        obj = in.substr(size);
                //        in += static_cast<int>(size + 1);
                //    }
                //};

                template<>
                struct helper<std::string>
                {
                    static void serialize(json_oarchive& out, const std::string& obj)
                    {
                        out.get() << '\"' << obj << '\"';
                    }

                    static void deserialize(json_iarchive& in, std::string& obj)
                    {
                        in++;
                        auto p = in.find_first_of('\"');
                        auto size = p - in.position();
                        obj = in.substr(size);
                        in += static_cast<int>(size + 1);
                    }

                };

            } // namespace fundamenta_tools

        } // namespace json

    } // namespace detail

} // namespace sl