#include "utils.hpp"

#include "base64.hpp"

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
