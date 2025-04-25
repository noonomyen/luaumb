#ifndef LUAUMB_BUNDLE_H
#define LUAUMB_BUNDLE_H

#include <string>
#include <vector>
#include <map>

#include "path.hpp"
#include "localizer.hpp"
#include "utils.hpp"

struct ModuleFile {
    std::string name;
    std::string source;
    std::vector<ExprCallRequire> requiress;
    RelativePathModule path;
};

class LuauModuleBundle {
    public:
        StrIntMap module_map_id;
        std::map<int, std::vector<int>> dependencies;
        std::map<std::string, ModuleFile> modules;
        RelativePathModule main_path;

        LuauModuleBundle(const RelativePathModule& main_path);
        bool is_loaded(const std::string& name);
        void set_module(const RelativePathModule& module_path, const std::string& source, const std::vector<ExprCallRequire>& requiress);
        void add_dependency(const std::string& a, const std::string& b);
        std::vector<std::string> load_order();
        std::string build();
};

#endif /* LUAUMB_BUNDLE_H */
