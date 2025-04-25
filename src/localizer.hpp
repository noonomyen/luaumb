#ifndef LUAUMB_LOCALIZER_H
#define LUAUMB_LOCALIZER_H

#include "Luau/Location.h"
#include "Luau/ParseResult.h"

struct Location {
    unsigned int begin_line, begin_column, end_line, end_column;

    Location(const Luau::Location& location);
    operator std::string() const;
};

struct ExprCallRequire {
    std::string name;
    std::string path;
    Location location;
};

struct RequireFunctionLocalizerResult {
    std::vector<Luau::ParseError> parse_errors;
    std::vector<ExprCallRequire> list;
};

RequireFunctionLocalizerResult require_function_localizer(const std::string& source);

#endif /* LUAUMB_LOCALIZER_H */
