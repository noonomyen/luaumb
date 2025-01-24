#include "utils.h"
#include "base64.hpp"

using namespace std;

namespace fs = filesystem;

RelativePathModule::RelativePathModule(const fs::path& root, const fs::path& relative, const fs::path& path) {
    this->root = root;
    this->relative = relative;
    this->path = path;
}

RelativePathModule::RelativePathModule(const fs::path& main_path) {
    this->path = main_path.lexically_normal();
    this->relative = main_path.filename();
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

    path += ".luau";
    if (!fs::exists(path)) {
        path.replace_extension(".lua");
        if (!fs::exists(path)) {
            throw runtime_error("Error module file not found\n  Parent: " + string(parent_path) + "\n  File: " + path.string());
        }
    }

    return RelativePathModule(parent_path.root, path.lexically_proximate(parent_path.root), path);
}

int StrIntMap::operator[](const string& key) {
    if (this->str_int.find(key) == this->str_int.end()) {
        this->str_int[key] = this->size;
        this->int_str[this->size] = key;
        this->size++;
    }

    return this->str_int[key];
}

string StrIntMap::operator[](const int& key) {
    if (this->int_str.find(key) == this->int_str.end()) {
        throw runtime_error("Error key not found in StrIntMap, Key: " + to_string(key));
    }

    return this->int_str[key];
}

string nonstd_base64::encode(const string& str) {
    string encoded = base64::to_base64(str);
    size_t count_ = count(encoded.rbegin(), encoded.rend(), '=');
    encoded = encoded.substr(0, encoded.size() - count_) + to_string(count_);

    return encoded;
}

string nonstd_base64::decode(const string& str) {
    size_t count_ = str.back() - '0';
    string encoded = str.substr(0, str.size() - 1);
    for (size_t i = 0; i < count_; i++) encoded += '=';

    return base64::from_base64(encoded);
}
