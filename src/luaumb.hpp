#ifndef LUAUMB_H
#define LUAUMB_H

#include <string>

#ifndef LUAUMB_VERSION
#define LUAUMB_VERSION "0.1-dev"
#endif

#ifndef LUAUMB_LUAU_VERSION
#define LUAUMB_LUAU_VERSION "0.658-release"
#endif

#ifndef LUAUMB_LIB_BASE64_VERSION
#define LUAUMB_LIB_BASE64_VERSION "387b32f-checkout"
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
