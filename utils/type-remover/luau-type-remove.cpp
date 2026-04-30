// WIP: This file is a work in progress and may contain incomplete or incorrect code. Use with caution.

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

#include "Luau/Ast.h"
#include "Luau/Location.h"
#include "Luau/Allocator.h"
#include "Luau/Parser.h"
#include "Luau/ParseOptions.h"
#include "Luau/ParseResult.h"

enum TypeAnnotationKind {
    None,
    PrevTypeAnnotationSeparator,
    FunctionGenericTypePack,
    PrevTypeAssertion
};

struct Annotations {
    Luau::Location location;
    std::optional<Luau::Location> replacementLocation = std::nullopt;
    TypeAnnotationKind kind = None;
};

void printLocation(const Luau::Location& location) {
    std::cout << location.begin.line+1 << ":" << location.begin.column+1 << "-" << location.end.line+1 << ":" << location.end.column+1 << std::endl;
}

struct FindTypeAnnotations : public Luau::AstVisitor {
    std::vector<Annotations> list;

    bool visit(Luau::AstNode* node) override {

        if (auto expr = node->asExpr()) {

            if (auto typeAssertion = expr->as<Luau::AstExprTypeAssertion>()) {

                if (auto annotationType = typeAssertion->annotation->asType()) {
                    // "Expr :: Type" -> "Expr"
                    std::cout << "AstExprTypeAssertion -> Annotation - ";
                    printLocation(annotationType->location);
                    list.push_back({ annotationType->location, std::nullopt, PrevTypeAssertion });
                }
            } else if (auto exprFunc = node->as<Luau::AstExprFunction>()) {

                if (exprFunc->returnAnnotation) {
                    std::optional<Luau::Location> loc;
                    if (auto annotationType = exprFunc->returnAnnotation->as<Luau::AstTypePackExplicit>()) {
                        // "function(...): Type" -> "function(...)"
                        std::cout << "AstExprFunction -> ReturnAnnotation -> AstTypePackExplicit - ";
                        loc = annotationType->location;
                    } else if (auto annotationType = exprFunc->returnAnnotation->as<Luau::AstTypePackVariadic>()) {
                        // "function(...): ...Type" -> "function(...)"
                        std::cout << "AstExprFunction -> ReturnAnnotation -> AstTypePackVariadic - ";
                        loc = annotationType->location;
                    } else if (auto annotationType = exprFunc->returnAnnotation->as<Luau::AstTypePackGeneric>()) {
                        // "function(...): ...T" -> "function(...)"
                        std::cout << "AstExprFunction -> ReturnAnnotation -> AstTypePackGeneric - ";
                        loc = annotationType->location;
                    }

                    if (loc.has_value()) {
                        printLocation(loc.value());
                        list.push_back({ loc.value(), std::nullopt, PrevTypeAnnotationSeparator });
                    }
                }

                if (exprFunc->generics.size > 0) {
                    auto generic = exprFunc->generics.data[0];
                    // "function<T>()" -> "function()"
                    std::cout << "AstExprFunction -> AstGenericType - " << generic->location.begin.line+1 << std::endl;
                    list.push_back({ generic->location, std::nullopt, FunctionGenericTypePack });
                }

                if (exprFunc->genericPacks.size > 0) {
                    auto generic = exprFunc->genericPacks.data[0];
                    // "function<T..., U...>()" -> "function()"
                    std::cout << "AstExprFunction -> AstGenericTypePack - " << generic->location.begin.line+1 << std::endl;
                    list.push_back({ generic->location, std::nullopt, FunctionGenericTypePack });
                }

                if (exprFunc->varargAnnotation) {
                    if (auto annotationType = exprFunc->varargAnnotation->as<Luau::AstTypePackGeneric>()) {
                        // "function(...: T...)" -> "function(...)"
                        std::cout << "AstExprFunction -> VarargAnnotation -> AstTypePackGeneric - ";
                        printLocation(annotationType->location);
                        list.push_back({ annotationType->location, std::nullopt, PrevTypeAnnotationSeparator });
                    }
                }

                if (exprFunc->args.size > 0) {
                    for (size_t i = 0; i < exprFunc->args.size; i++) {
                        auto arg = exprFunc->args.data[i];
                        if (arg->annotation) {
                            if (auto annotationType = arg->annotation->asType()) {
                                // "function(arg1: Type, arg2: Type)" -> "function(arg1, arg2)"
                                std::cout << "AstExprFunction -> Args -> Annotation - ";
                                printLocation(annotationType->location);
                                list.push_back({ annotationType->location, std::nullopt, PrevTypeAnnotationSeparator });
                            }
                        }
                    }
                }
            }
        } else if (auto stat = node->asStat()) {

            if (auto local = stat->as<Luau::AstStatTypeAlias>()) {
                // "type Name = Type" -> ""
                std::cout << "AstStatTypeAlias - ";
                printLocation(local->location);
                list.push_back({ local->location, std::nullopt });
            } else if (auto statLocal = stat->as<Luau::AstStatLocal>()) {

                for (size_t i = 0; i < statLocal->vars.size; i++) {
                    auto var = statLocal->vars.data[i];
                    if (var->annotation) {
                        if (auto annotationType = var->annotation->asType()) {
                            // "local var: Type" -> "local var"
                            std::cout << "AstStatLocal -> Vars -> Annotation - ";
                            printLocation(annotationType->location);
                            list.push_back({ annotationType->location, std::nullopt, PrevTypeAnnotationSeparator });
                        }
                    }
                }
            }
        }

        return true;
    }
};

int main(int argc, char *argv[]) {
    if ((argc < 3) || (argc >= 3 && strcmp(argv[1], "--help") == 0)) {
        std::cout << "Luau type remover" << std::endl;
        std::cout << "  luau-type-remove [src.luau] [dist.luau]" << std::endl;

        return 0;
    }

    // read source file into string

    std::string source_str;
    std::ifstream in_file(argv[1]);
    if (!in_file) {
        std::cerr << "Failed to open input file: " << argv[1] << std::endl;
        return 1;
    }

    source_str.assign((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
    in_file.close();

    // parse source file and find type annotations

    Luau::Allocator allocator;
    Luau::AstNameTable names(allocator);

    Luau::ParseOptions options;
    options.captureComments = true;
    options.allowDeclarationSyntax = true;

    Luau::ParseResult result = Luau::Parser::parse(source_str.data(), source_str.size(), names, allocator, options);

    FindTypeAnnotations search;
    result.root->visit(&search);

    // split source into lines for easier

    std::vector<std::string> source;
    std::string current_line;
    for (const auto& c : source_str) {
        if (c == '\n') {
            source.push_back(current_line);
            current_line.clear();
        } else {
            current_line += c;
        }
    }
    source.push_back(current_line);

    // remove type annotations from source

    std::sort(search.list.begin(), search.list.end(), [](const Annotations& a, const Annotations& b) {
        if (a.location.begin.line == b.location.begin.line) return a.location.begin.column < b.location.begin.column;
        return a.location.begin.line < b.location.begin.line;
    });

    std::string output;
    std::pair<int, int> last_location = {0, 0}; // first - line, second - column

    // get replacement string
    for (auto annotation : search.list) {
        std::string replacement = "";
        if (annotation.replacementLocation.has_value()) {
            auto rep_loc = annotation.replacementLocation.value();

            if (rep_loc.begin.line == rep_loc.end.line) {
                replacement = source[rep_loc.begin.line].substr(rep_loc.begin.column, rep_loc.end.column - rep_loc.begin.column);
            } else {
                replacement = source[rep_loc.begin.line].substr(rep_loc.begin.column) + "\n";
                for (int i = rep_loc.begin.line + 1; i < rep_loc.end.line; i++) replacement += source[i] + "\n";
                replacement += source[rep_loc.end.line].substr(0, rep_loc.end.column);
            }
        }

        auto loc = annotation.location;

        // hardcoded handling for different annotation kinds to find correct location to remove.
        // find previous ":" for function return type annotations
        if (annotation.kind == PrevTypeAnnotationSeparator) {
            int line = loc.begin.line;
            int column = loc.begin.column;
            while (line >= 0) {
                if (source[line][column] == ':') {
                    loc.begin.column = column;
                    loc.begin.line = line;
                    break;
                }
                column--;
                if (column < 0) {
                    line--;
                    if (line >= 0) column = source[line].size() - 1;
                }
            }
        } else if (annotation.kind == FunctionGenericTypePack) {
            // find previous "<" for function generic type packs
            int line = loc.begin.line;
            int column = loc.begin.column;
            while (line >= 0) {
                if (source[line][column] == '<') {
                    loc.begin.column = column;
                    loc.begin.line = line;
                    break;
                }
                column--;
                if (column < 0) {
                    line--;
                    if (line >= 0) column = source[line].size() - 1;
                }
            }
            // find next ">" for function generic type packs
            line = loc.end.line;
            column = loc.end.column;
            while (line < source.size()) {
                if (source[line][column] == '>') {
                    loc.end.column = column + 1;
                    loc.end.line = line;
                    break;
                }
                column++;
                if (column >= source[line].size()) {
                    line++;
                    column = 0;
                }
            }
        } else if (annotation.kind == PrevTypeAssertion) {
            // find previous "::" for type assertions
            int line = loc.begin.line;
            int column = loc.begin.column;
            while (line >= 0) {
                if (source[line][column] == ':' && column > 0 && source[line][column - 1] == ':') {
                    loc.begin.column = column - 1;
                    loc.begin.line = line;
                    break;
                }
                column--;
                if (column < 0) {
                    line--;
                    if (line >= 0) column = source[line].size() - 1;
                }
            }
        }

        // fill output with source code from last location to current annotation location, then add replacement string
        if (loc.begin.line == last_location.first) {
            output += source[last_location.first].substr(last_location.second, loc.begin.column - last_location.second);
        } else {
            output += source[last_location.first].substr(last_location.second) + "\n";
            for (int i = last_location.first + 1; i < loc.begin.line; i++) output += source[i] + "\n";
            output += source[loc.begin.line].substr(0, loc.begin.column);
        }

        output += replacement;
        last_location = {loc.end.line, loc.end.column};
    }
    output += source[last_location.first].substr(last_location.second);
    for (int i = last_location.first + 1; i < source.size(); i++) output += "\n" + source[i];

    // write output to file

    std::ofstream out_file(argv[2]);
    if (!out_file) {
        std::cerr << "Failed to open output file: " << argv[2] << std::endl;
        return 1;
    }
    out_file << output;
    out_file.close();

    std::cout << "Type annotations removed " << search.list.size() << " nodes." << std::endl;

    return 0;
}
