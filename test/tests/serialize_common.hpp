
namespace sl
{

    namespace test
    {

        template<typename T, typename IN, typename OUT>
        struct serialize_common
        {
            static void apply(const T& original)
            {
                T copy;
                std::stringstream stream;
                sl::save<std::stringstream, OUT, T>(stream, original);
                sl::load<std::stringstream, IN, T>(stream, copy);
                original == copy;
            }
        };

    } // namespace test

} // namespace sl