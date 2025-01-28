#include <iostream>
#include <string>
#include <cstring>

#include "luaumb.hpp"

int main(int argc, char** argv) {
    if ((argc < 3) || (argc >= 3 && strcmp(argv[1], "--help") == 0)) {
        std::cout << "Luau module bundle - " << luaumb::version.luaumb << " luau:" << luaumb::version.luau << " base64:" << luaumb::version.base64 << std::endl;
        std::cout << "  luaumb [main.luau] [out.luau]" << std::endl;

        return 0;
    }

    const std::string in_file = argv[1];
    const std::string out_file = argv[2];

    luaumb::bundle(in_file, out_file);

    return 0;
}
