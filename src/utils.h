#ifndef LUAUMB_UTILS_H
#define LUAUMB_UTILS_H

#include <filesystem>
#include <map>
#include <string>

struct RelativePathModule {
    std::filesystem::path root;
    std::filesystem::path relative;
    std::filesystem::path path;

    RelativePathModule() = default;
    RelativePathModule(const std::filesystem::path& root, const std::filesystem::path& relative, const std::filesystem::path& path);
    RelativePathModule(const std::filesystem::path& main_path);
    operator std::string() const;
};;

RelativePathModule relative_path_module(const RelativePathModule& parent_path, const std::filesystem::path& module_path);

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
