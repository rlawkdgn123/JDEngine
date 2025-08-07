#pragma once
#include <string>

namespace JDUtil {
    inline std::string trim(const std::string& str)
    {
        const char* whitespace = " \t\n\r\f\v";
        size_t first = str.find_first_not_of(whitespace);
        if (std::string::npos == first)
        {
            return ""; // ���ڿ� ��ü�� ������ ���
        }
        size_t last = str.find_last_not_of(whitespace);
        return str.substr(first, (last - first + 1));
    }

    JDGlobal::Contents::Resource GetCatBonus(JDGlobal::Contents::CatType cat);
}

