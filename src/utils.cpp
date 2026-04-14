#include "utils.hpp"

#include <stdexcept>

using namespace std;

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
