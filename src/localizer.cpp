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

struct L_AstExprGlobal : public Luau::AstVisitor {
    bool ok = false;

    bool visit(class Luau::AstExprGlobal* node) {
        if (strncmp(node->name.value, "require", 7) == 0) ok = true;

        return true;
    }
};

struct L_AstExprConstantString : public Luau::AstVisitor {
    bool ok = false;
    std::string value;

    bool visit(class Luau::AstExprConstantString* node) {
        this->value = node->value.data;
        ok = true;

        return true;
    }
};

struct L_AstExprCall : public Luau::AstVisitor {
    std::vector<ExprCallRequire> list;

    bool visit(class Luau::AstExprCall* node) override {
        L_AstExprGlobal ast_expr_global;
        node->visit(&ast_expr_global);

        if (node->args.size == 1) {
            L_AstExprConstantString ast_expr_constant_string;
            node->args.data[0]->visit(&ast_expr_constant_string);

            if (ast_expr_global.ok && ast_expr_constant_string.ok) this->list.push_back({"", ast_expr_constant_string.value, Location(node->location)});
        }

        return false;
    }
};

RequireFunctionLocalizerResult require_function_localizer(const std::string& source) {
    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);

    Luau::ParseOptions options;
    options.captureComments = true;
    options.allowDeclarationSyntax = true;

    Luau::ParseResult result = Luau::Parser::parse(source.data(), source.size(), names, allocator, options);

    L_AstExprCall search;
    result.root->visit(&search);

    return {result.errors, search.list};
}
