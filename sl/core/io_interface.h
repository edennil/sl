#pragma once

#include <string>

namespace sl
{

    template<typename STREAM, typename ARCHIVE, typename OBJECT>
    void load(const std::string &file, OBJECT &obj)
    {
        STREAM ifs(file);
        ARCHIVE in(ifs);
        in >> obj;
    }

    template<typename STREAM, typename ARCHIVE, typename OBJECT>
    void save(const std::string &file, const OBJECT &obj)
    {
        STREAM ifs(file);
        ARCHIVE out(ifs);
        out << obj;
    }

    template<typename STREAM, typename ARCHIVE, typename OBJECT>
    void load(STREAM &ifs, OBJECT &obj)
    {
        ARCHIVE in(ifs);
        in >> obj;
    }

    template<typename STREAM, typename ARCHIVE, typename OBJECT>
    void save(STREAM &ofs, const OBJECT &obj)
    {
        ARCHIVE out(ofs);
        out << obj;
    }
    
} // namespace sl
