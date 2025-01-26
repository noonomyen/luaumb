#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <filesystem>
#include <optional>
#include <fstream>

#include "Luau/ToString.h"
#include "Luau/FileUtils.h"
#include "Luau/Config.h"

#include "localizer.h"
#include "bundle.h"
#include "utils.h"

#ifndef LUAUMB_VERSION
#define LUAUMB_VERSION "0.1"
#endif

int main(int argc, char** argv) {
    luau_fvalue_init();

    if ((argc < 3) || (argc >= 3 && strcmp(argv[1], "--help") == 0)) {
        std::cout << "Luau module bundle - v" << LUAUMB_VERSION << std::endl;
        std::cout << "  luaumb [main.luau] [out.luau]" << std::endl;
        return 0;
    }

    const char* in_name = argv[1];
    const char* out_name = argv[2];

    std::vector<Luau::ParseError> errors;
    std::map<std::string, Luau::Config> configs;
    RelativePathModule main_path(in_name);
    LuauModuleBundle lmb(main_path);
    std::queue<RelativePathModule> q;
    q.push(main_path);

    while (q.size() > 0) {
        RelativePathModule module_path = q.front();
        q.pop();

        if (lmb.is_loaded(module_path.relative.string())) continue;

        std::optional<std::string> file = readFile(module_path.path.string());

        if (!file) {
            std::cout << "Couldn't read source " << module_path.path << std::endl;
            return 1;
        }

        const std::string config_file_relative_path = module_path.relative.parent_path().string();

        if (configs.find(config_file_relative_path) == configs.end()) {
            Luau::Config config;

            std::filesystem::path relative = module_path.relative.parent_path().parent_path();
            while (relative.string() != "/") {
                const auto it = configs.find(relative.string());
                if (it != configs.end()) {
                    config = Luau::Config(it->second);
                    break;
                } else {
                    relative = relative.parent_path();
                }
            }

            if (std::optional<std::string> config_file = readFile(module_path.path.replace_filename(Luau::kConfigName).string())) {
                Luau::ConfigOptions config_option = {false, std::optional<Luau::ConfigOptions::AliasOptions>({config_file_relative_path, true})};
                std::optional<std::string> error = Luau::parseConfig(*config_file, config, config_option);

                if (error) throw std::runtime_error("Error parsing config: " + *error + "\n  File: " + module_path.path.string());
            }

            configs[config_file_relative_path] = config;
        }

        const Luau::Config& config = configs[config_file_relative_path];

        RequireFunctionLocalizerResult result = require_function_localizer(*file);
        errors.insert(errors.end(), result.parse_errors.begin(), result.parse_errors.end());

        for (ExprCallRequire& require : result.list) {
            std::filesystem::path path;
            if (require.path.rfind("./", 0) == 0 || require.path.rfind("../", 0) == 0) {
                path = require.path;
            } else if (require.path.rfind("@", 0) == 0) {
                const auto alias_info = config.aliases.find(require.path.substr(1));
                if (alias_info == NULL) throw std::runtime_error("Error, is not a valid alias: " + require.path);
                path = alias_info->configLocation;
                path = path.lexically_relative(module_path.relative.parent_path()).relative_path() / std::filesystem::path(alias_info->value);
            } else {
                continue;
            }

            RelativePathModule next = relative_path_module(module_path, path);
            require.name = next.relative.string();
            lmb.add_dependency(module_path.relative.string(), next.relative.string());
            q.push(next);
        }

        lmb.set_module(module_path, *file, result.list);
    }

    std::cout << "Modules and dependencies" << std::endl;
    for (const std::string& name : lmb.load_order()) {
        const ModuleFile& module = lmb.modules[name];
        std::cout << "  " << name << "" << std::endl;
        for (const ExprCallRequire& require : module.requiress) {
            std::cout << "  └─ [" << require.path << "] (" << require.name << ") -- " << std::string(require.location) << std::endl;
        }
    }

    if (errors.size() > 0) {
        std::cout << "Parse errors were encountered:" << std::endl;
        for (const Luau::ParseError& error : errors) {
            std::cout << "  " << toString(error.getLocation()) << " - " << error.getMessage() << std::endl;
        }
    }

    const std::string out = lmb.build();
    std::ofstream out_file(out_name);
    if (!out_file) {
        std::cout << "Couldn't open output file " << out_name << std::endl;
        return 1;
    }
    out_file << out;
    out_file.close();

    return errors.size() > 0 ? 1 : 0;
}
