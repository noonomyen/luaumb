#include "path.hpp"

#include <array>

using namespace std;

namespace fs = filesystem;

const std::array<const string, 4> module_suffixes = {".luau", ".lua", "/init.luau", "/init.lua"};

RelativePathModule::RelativePathModule(const fs::path& root, const fs::path& relative, const fs::path& path) {
    this->root = root;
    this->relative = relative.is_absolute() ? relative : fs::path("/") / relative;
    this->path = path;
}

RelativePathModule::RelativePathModule(const fs::path& main_path) {
    this->path = main_path.lexically_normal();
    fs::path root_relative("/");
    root_relative += main_path.filename();
    this->relative = root_relative;
    this->root = main_path.parent_path();

    if (!fs::exists(this->path)) {
        throw runtime_error("Error main file not found\n  File: " + this->path.string());
    }
}

RelativePathModule::operator string() const {
    stringstream ss;
    ss << "[ROOT:\"" << this->root.string() << "\" PATH:\"" << this->path.string() << "\" RELATIVE:\"" << this->relative.string() << "\"]";

    return ss.str();
}

RelativePathModule relative_path_module(const RelativePathModule& parent_path, const fs::path& module_path) {
    fs::path path = (parent_path.path.parent_path() / module_path).lexically_normal();

    bool found = false;
    for (string ext : module_suffixes) {
        fs::path _path = path;
        _path += ext;
        if (fs::exists(_path)) {
            found = true;
            path = _path;
            break;
        }
    }

    if (!found) throw runtime_error("Error module file not found\n  Parent: " + string(parent_path) + "\n  File: " + path.string());

    return RelativePathModule(parent_path.root, path.lexically_proximate(parent_path.root), path);
}
