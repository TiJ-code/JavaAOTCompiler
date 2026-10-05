#include "jaot/semantic.h"

#include <unordered_map>
#include <unordered_set>

namespace JAOT {
    namespace {
        using MethodTable = std::unordered_map<std::string, const Method *>;
        using LocalNames = std::unordered_set<std::string>;

        void semanticError(SourceLocation location, const std::string &message) {
            throw std::runtime_error("semantic error at "
                + std::to_string(location.line)
                + ":"
                + std::to_string(location.column)
                + ": " + message);
        }

        Type checkExpression(const Expr &expression, const LocalNames &locals, const MethodTable &methods) {
            switch (expression.kind) {
                case ExprKind::Integer:
                    return Type::Int;

                case ExprKind::Variable:
                    if (!locals.contains(expression.name)) {
                        semanticError(
                            expression.location,
                            "unknown variable: " + expression.name);
                    }
                    return Type::Int;

                case ExprKind::Binary: {
                    const Type left =
                            checkExpression(*expression.left, locals, methods);
                    const Type right =
                            checkExpression(*expression.right, locals, methods);

                    if (left != Type::Int || right != Type::Int) {
                        semanticError(
                            expression.location,
                            "arithmetic operands must have type int");
                    }
                    return Type::Int;
                }

                case ExprKind::Call: {
                    if (expression.callee == "printInt") {
                        if (expression.arguments.size() != 1) {
                            semanticError(
                                expression.location,
                                "printInt expects one argument");
                        }

                        const Type argument = checkExpression(
                            *expression.arguments[0], locals, methods);
                        if (argument != Type::Int) {
                            semanticError(
                                expression.arguments[0]->location,
                                "printInt argument must have type int");
                        }
                        return Type::Void;
                    }

                    const auto found = methods.find(expression.callee);
                    if (found == methods.end()) {
                        semanticError(
                            expression.location,
                            "unknown function: " + expression.callee);
                    }

                    const Method &callee = *found->second;
                    if (expression.arguments.size() != callee.parameters.size()) {
                        semanticError(
                            expression.location,
                            "function " + expression.callee + " expects " +
                            std::to_string(callee.parameters.size()) +
                            " arguments");
                    }

                    for (std::size_t i = 0;
                         i < expression.arguments.size();
                         ++i) {
                        const Expr &argument = *expression.arguments[i];
                        const Type argumentType =
                                checkExpression(argument, locals, methods);

                        if (argumentType != callee.parameters[i].type) {
                            semanticError(
                                argument.location,
                                "argument type does not match parameter '" +
                                callee.parameters[i].name + "'");
                        }
                    }

                    return callee.returnType;
                }
            }

            throw std::runtime_error("unknown expression kind");
        }

        void checkMethod(const Method &method, const MethodTable &methods) {
            LocalNames locals;

            for (const Parameter &parameter : method.parameters) {
                if (!locals.insert(parameter.name).second) {
                    semanticError(
                        parameter.location,
                        "duplicate parameter: " + parameter.name
                        );
                }
            }

            bool hasReturn = false;

            for (const Stmt &statement : method.body) {
                switch (statement.kind) {
                    case StmtKind::VarDecl: {
                        if (locals.contains(statement.name)) {
                            semanticError(
                                statement.location,
                                "duplicate variable: " + statement.name
                            );
                        }

                        const Type initializerType = checkExpression(*statement.expression, locals, methods);
                        if (initializerType != Type::Int) {
                            semanticError(
                                statement.location,
                                "variable initializer must have type int"
                            );
                        }

                        locals.insert(statement.name);
                        break;
                    }

                    case StmtKind::Expression:
                        checkExpression(*statement.expression, locals, methods);
                        break;

                    case StmtKind::Return:
                        hasReturn = true;

                        if (method.returnType == Type::Void) {
                            if (statement.expression) {
                                semanticError(
                                    statement.location,
                                    "void method cannot return a value"
                                );
                            }
                        } else {
                            if (!statement.expression) {
                                semanticError(
                                    statement.location,
                                    "int method must return a value"
                                );
                            }

                            const Type returnType = checkExpression(*statement.expression, locals, methods);
                            if (returnType != Type::Int) {
                                semanticError(statement.expression->location, "return expression must have type int");
                            }
                        }
                        break;
                }
            }

            if (method.returnType == Type::Int && !hasReturn) {
                semanticError(method.location, "int method must return a value");
            }
        }
    }

    void SematicAnalyzer::analyze(const Program &program) const {
        MethodTable methods;

        for (const Method &method : program.methods) {
            if (method.name == "printInt") {
                semanticError(
                    method.location,
                    "method name 'printInt' is reserved"
                );
            }

            if (!methods.emplace(method.name, &method).second) {
                semanticError(
                    method.location,
                    "duplicate function: " + method.name
                );
            }
        }

        const auto entryPoint = methods.find("main");
        if (entryPoint == methods.end()) {
            throw std::runtime_error(
                "semantic error: missing required entry point 'main' (expected static void main())"
            );
        }

        const Method &mainMethod = *entryPoint->second;
        if (mainMethod.returnType != Type::Void || !mainMethod.parameters.empty()) {
            semanticError(
                mainMethod.location,
                "entry point 'main' must have signature 'static void main()'"
            );
        }

        for (const Method &method : program.methods) {
            checkMethod(method, methods);
        }
    }
}
