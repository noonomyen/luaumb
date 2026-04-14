#include "bundle.hpp"

#include <sstream>
#include <queue>
#include <map>

using namespace std;

LuauModuleBundle::LuauModuleBundle(const RelativePathModule& main_path) : main_path(main_path) {}

bool LuauModuleBundle::is_loaded(const string& name) {
    return this->modules.find(name) != this->modules.end();
}

void LuauModuleBundle::set_module(const RelativePathModule& module_path, const string& source, const vector<ExprCallRequire>& requiress) {
    this->modules[module_path.relative.string()] = {module_path.relative.string(), source, requiress, module_path};
}

void LuauModuleBundle::add_dependency(const string& a, const string& b) {
    this->dependencies[this->module_map_id[a]].push_back(this->module_map_id[b]);
}

vector<string> LuauModuleBundle::load_order() {
    // Topological Sorting - Kahn's

    const size_t size = this->module_map_id.size;

    vector<vector<int>> adj(size);
    vector<int> indeg(size);
    queue<int> q;
    vector<int> result;
    vector<string> result_str(size);

    for (size_t i = 0; i < size; i++) adj[i] = this->dependencies[i];
    for (size_t i = 0; i < size; i++) for (auto it : adj[i]) indeg[it]++;
    for (size_t i = 0; i < size; i++) if (indeg[i] == 0) q.push(i);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        result.push_back(node);

        for (auto it : adj[node]) {
            indeg[it]--;
            if (indeg[it] == 0) q.push(it);
        }
    }

    if (result.size() != size) throw runtime_error("Error cyclic module dependency");
    for (size_t i = 0; i < size; i++) result_str[size - i - 1] = this->module_map_id[result[i]];

    return result_str;
}

string LuauModuleBundle::build() {
    const vector<string> order = (this->modules.size() == 1) ? vector<string>{this->modules.begin()->first} : this->load_order();
    map<string, string> mapped_order;

    int i = 0;
    for (string name : order) mapped_order[name] = to_string(i++);

    if (mapped_order.find(this->main_path.relative.string()) == mapped_order.end()) throw runtime_error("Error main module not found");

    string out;

    out += "local __LUAUMB = {}\n";
    out += "__LUAUMB.METATABLE = { __index = getfenv() }\n";
    out += "__LUAUMB.MODULES = {}\n";
    out += "__LUAUMB.LOADED = {}\n";
    out += "__LUAUMB.LOAD = function(self, module_index)\n";
    out += "local module = self.MODULES[module_index]\n";
    out += "if module == nil then return self.LOADED[module_index] end\n";
    out += "local loaded = setfenv(module, setmetatable({}, self.METATABLE))()\n";
    out += "self.MODULES[module_index] = nil\n";
    out += "self.LOADED[module_index] = loaded\n";
    out += "return loaded\n";
    out += "end\n";

    for (string name : order) {
        const ModuleFile& module = this->modules[name];

        out += "-- " + module.path.relative.string() + "\n";
        out += "__LUAUMB.MODULES[" + mapped_order[name] + "] = function()\n";

        vector<string> lines;
        stringstream ss(module.source);
        string line;

        while (getline(ss, line, '\n')) lines.push_back(line + "\n");

        unsigned int pos_line = 0;
        unsigned int pos_col = 0;

        for (const ExprCallRequire& require : module.requiress) {
            const string call = "__LUAUMB:LOAD(" + mapped_order[require.name] + ")";
            const Location& location = require.location;

            if (location.begin_line == pos_line) {
                out += lines[pos_line].substr(pos_col, location.begin_column) + call;
            } else {
                out += lines[pos_line++].substr(pos_col);
                pos_col = 0;
                for (unsigned int i = pos_line; i < location.begin_line; i++) out += lines[i];
                out += lines[location.begin_line].substr(pos_col, location.begin_column) + call;
            }

            pos_line = location.end_line;
            pos_col = location.end_column;
        }

        out += lines[pos_line++].substr(pos_col);
        if (pos_line < lines.size()) for (unsigned int i = pos_line; i < lines.size(); i++) out += lines[i];
        out += "\nend\n";
    }

    out += "__LUAUMB:LOAD(" + mapped_order[this->main_path.relative.string()] + ")";

    return out;
}
