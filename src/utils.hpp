#ifndef LUAUMB_UTILS_H
#define LUAUMB_UTILS_H

#include <string>
#include <map>

class StrIntMap {
    public:
        std::map<std::string, int> str_int;
        std::map<int, std::string> int_str;
        size_t size = 0;

        int operator[](const std::string& key);
        std::string operator[](const int& key);
};

namespace nonstd_base64 {
    std::string encode(const std::string& str);
    std::string decode(const std::string& str);
}

#endif /* LUAUMB_UTILS_H */
