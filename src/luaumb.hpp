#ifndef LUAUMB_H
#define LUAUMB_H

#include <string>

#ifndef LUAUMB_VERSION
#define LUAUMB_VERSION "0.1"
#endif

#ifndef LUAUMB_LUAU_VERSION
#define LUAUMB_LUAU_VERSION "0.671"
#endif

#ifndef LUAUMB_LIB_BASE64_VERSION
#define LUAUMB_LIB_BASE64_VERSION "0d0f5a8"
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
