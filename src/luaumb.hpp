#ifndef LUAUMB_H
#define LUAUMB_H

#include <string>

#ifndef LUAUMB_VERSION
#define LUAUMB_VERSION "0.2"
#endif

#ifndef LUAUMB_LUAU_VERSION
#define LUAUMB_LUAU_VERSION "0.715"
#endif

#ifndef LUAUMB_LIB_BASE64_VERSION
#define LUAUMB_LIB_BASE64_VERSION "8d96a2a"
#endif

namespace luaumb {
    const struct {
        const std::string luaumb = LUAUMB_VERSION;
        const std::string luau = LUAUMB_LUAU_VERSION;
        const std::string base64 = LUAUMB_LIB_BASE64_VERSION;
    } version;

    void bundle(const std::string& main_file, const std::string& out_file);
}

#endif /* LUAUMB_H */
