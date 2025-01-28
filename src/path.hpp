#ifndef LUAUMB_PATH_HPP
#define LUAUMB_PATH_HPP

#include <string>
#include <filesystem>

struct RelativePathModule {
    std::filesystem::path root;
    std::filesystem::path relative;
    std::filesystem::path path;

    RelativePathModule() = default;
    RelativePathModule(const std::filesystem::path& root, const std::filesystem::path& relative, const std::filesystem::path& path);
    RelativePathModule(const std::filesystem::path& main_path);
    operator std::string() const;
};

RelativePathModule relative_path_module(const RelativePathModule& parent_path, const std::filesystem::path& module_path);

#endif /* LUAUMB_PATH_HPP */
