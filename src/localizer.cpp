#include "localizer.h"

#include <string>
#include <sstream>

#include "Luau/Ast.h"
#include "Luau/Location.h"
#include "Luau/Allocator.h"
#include "Luau/Lexer.h"
#include "Luau/Parser.h"
#include "Luau/ParseOptions.h"
#include "Luau/Common.h"

Location::Location(const Luau::Location& location) {
    this->begin_line = location.begin.line;
    this->begin_column = location.begin.column;
    this->end_line = location.end.line;
    this->end_column = location.end.column;
}

Location::operator std::string() const {
    std::stringstream ss;
    ss << begin_line << ":" << begin_column << "-" << end_line << ":" << end_column;

    return ss.str();
}

void luau_fvalue_init() {
    for (Luau::FValue<bool>* flag = Luau::FValue<bool>::list; flag; flag = flag->next) {
        if (strncmp(flag->name, "Luau", 4) == 0) flag->value = true;
    }
}

struct FindRequireConstantString : public Luau::AstVisitor {
    std::vector<ExprCallRequire> list;

    bool visit(class Luau::AstNode* node) override {
        if (Luau::AstExprCall* call_node = dynamic_cast<Luau::AstExprCall*>(node)) {
            Luau::AstExpr* func = call_node->func;

            if (Luau::AstExprGlobal* func_node = dynamic_cast<Luau::AstExprGlobal*>(func)) {
                if (strncmp(func_node->name.value, "require", 7) == 0) {
                    Luau::AstExpr* args_0 = call_node->args.data[0];

                    if (Luau::AstExprConstantString* const_string_node = dynamic_cast<Luau::AstExprConstantString*>(args_0)) {
                        this->list.push_back({"", const_string_node->value.data, Location(call_node->location)});
                    }
                }
            }
        }
        return true;
    }
};

RequireFunctionLocalizerResult require_function_localizer(const std::string& source) {
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);

    Luau::ParseOptions options;
    options.captureComments = true;
    options.allowDeclarationSyntax = true;

    Luau::ParseResult result = Luau::Parser::parse(source.data(), source.size(), names, allocator, options);

    FindRequireConstantString search;
    result.root->visit(&search);

    return {result.errors, search.list};
}
