#include "jaot/semantic.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace JAOT {
    namespace {
        using MethodTable = std::unordered_map<std::string, const Method *>;
        using Initialized = std::unordered_set<std::size_t>;

        struct Symbol {
            Type type;
            std::size_t id;
        };

        using Scope = std::unordered_map<std::string, Symbol>;
        using Scopes = std::vector<Scope>;

        struct Flow {
            Initialized initialized;
            bool fallsThrough = true;
        };

        [[noreturn]] void semanticError(
            SourceLocation location,
            const std::string &message) {
            throw std::runtime_error(
                "semantic error at " + std::to_string(location.line) + ":" +
                std::to_string(location.column) + ": " + message);
        }

        const Symbol *findSymbol(const Scopes &scopes, const std::string &name) {
            for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
                const auto found = scope->find(name);
                if (found != scope->end()) {
                    return &found->second;
                }
            }
            return nullptr;
        }

        Type checkExpression(
            const Expr &expression,
            const Scopes &scopes,
            const Initialized &initialized,
            const MethodTable &methods) {
            switch (expression.kind) {
                case ExprKind::Integer:
                    return Type::Int;

                case ExprKind::Boolean:
                    return Type::Boolean;

                case ExprKind::Variable: {
                    const Symbol *symbol = findSymbol(scopes, expression.name);
                    if (symbol == nullptr) {
                        semanticError(
                            expression.location,
                            "unknown variable: " + expression.name);
                    }
                    if (!initialized.contains(symbol->id)) {
                        semanticError(
                            expression.location,
                            "variable used before initialization: " + expression.name);
                    }
                    return symbol->type;
                }

                case ExprKind::Binary: {
                    const Type left = checkExpression(
                        *expression.left, scopes, initialized, methods);
                    const Type right = checkExpression(
                        *expression.right, scopes, initialized, methods);

                    if (expression.op == "+" || expression.op == "-" ||
                        expression.op == "*" || expression.op == "/") {
                        if (left != Type::Int || right != Type::Int) {
                            semanticError(
                                expression.location,
                                "arithmetic operands must have type int");
                        }
                        return Type::Int;
                    }

                    if (expression.op == "<" || expression.op == "<=" ||
                        expression.op == ">" || expression.op == ">=") {
                        if (left != Type::Int || right != Type::Int) {
                            semanticError(
                                expression.location,
                                "relational operands must have type int");
                        }
                        return Type::Boolean;
                    }

                    if (expression.op == "==" || expression.op == "!=") {
                        if (left != right ||
                            (left != Type::Int && left != Type::Boolean)) {
                            semanticError(
                                expression.location,
                                "equality operands must have the same value type");
                        }
                        return Type::Boolean;
                    }

                    if (expression.op == "&&" || expression.op == "||") {
                        if (left != Type::Boolean || right != Type::Boolean) {
                            semanticError(expression.location, "logical operators require boolean operands");
                        }

                        return Type::Boolean;
                    }

                    semanticError(expression.location, "unknown binary operator");
                }

                case ExprKind::Unary: {
                    const Type operandType = checkExpression(*expression.operand, scopes, initialized, methods);

                    if (operandType != Type::Boolean) {
                        semanticError(expression.location, "logical negation requires a boolean operand");
                    }

                    return Type::Boolean;
                }

                case ExprKind::Call: {
                    if (expression.callee == "printInt") {
                        if (expression.arguments.size() != 1) {
                            semanticError(
                                expression.location,
                                "printInt expects one argument");
                        }
                        if (checkExpression(
                                *expression.arguments[0],
                                scopes,
                                initialized,
                                methods) != Type::Int) {
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
                            std::to_string(callee.parameters.size()) + " arguments");
                    }

                    for (std::size_t i = 0; i < expression.arguments.size(); ++i) {
                        const Type argumentType = checkExpression(
                            *expression.arguments[i],
                            scopes,
                            initialized,
                            methods);
                        if (argumentType != callee.parameters[i].type) {
                            semanticError(
                                expression.arguments[i]->location,
                                "argument type does not match parameter '" +
                                callee.parameters[i].name + "'");
                        }
                    }

                    return callee.returnType;
                }
            }

            throw std::runtime_error("unknown expression kind");
        }

        Flow analyzeStatements(
            const std::vector<Stmt> &statements,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const Method &method,
            std::size_t &nextSymbolId);

        Flow analyzeBlock(
            const std::vector<Stmt> &statements,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const Method &method,
            std::size_t &nextSymbolId) {
            scopes.emplace_back();
            flow = analyzeStatements(
                statements, std::move(flow), scopes, methods, method, nextSymbolId);

            for (const auto &[name, symbol] : scopes.back()) {
                (void)name;
                flow.initialized.erase(symbol.id);
            }
            scopes.pop_back();
            return flow;
        }

        Flow analyzeStatement(
            const Stmt &statement,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const Method &method,
            std::size_t &nextSymbolId) {
            if (!flow.fallsThrough) {
                return flow;
            }

            switch (statement.kind) {
                case StmtKind::VarDecl: {
                    if (scopes.back().contains(statement.name)) {
                        semanticError(
                            statement.location,
                            "duplicate variable: " + statement.name);
                    }

                    if (statement.expression) {
                        const Type initializerType = checkExpression(
                            *statement.expression,
                            scopes,
                            flow.initialized,
                            methods);
                        if (initializerType != statement.declaredType) {
                            semanticError(
                                statement.expression->location,
                                "variable initializer must have type " +
                                std::string(statement.declaredType == Type::Int
                                    ? "int" : "boolean"));
                        }
                    }

                    const std::size_t id = nextSymbolId++;
                    scopes.back().emplace(
                        statement.name, Symbol{statement.declaredType, id});
                    if (statement.expression) {
                        flow.initialized.insert(id);
                    }
                    return flow;
                }

                case StmtKind::Assignment: {
                    const Symbol *symbol = findSymbol(scopes, statement.name);
                    if (symbol == nullptr) {
                        semanticError(
                            statement.location,
                            "unknown variable: " + statement.name);
                    }

                    const Type valueType = checkExpression(
                        *statement.expression, scopes, flow.initialized, methods);
                    if (valueType != symbol->type) {
                        semanticError(
                            statement.expression->location,
                            "assignment value must have the same type as its target");
                    }
                    flow.initialized.insert(symbol->id);
                    return flow;
                }

                case StmtKind::Expression:
                    checkExpression(
                        *statement.expression, scopes, flow.initialized, methods);
                    return flow;

                case StmtKind::Return:
                    if (method.returnType == Type::Void) {
                        if (statement.expression) {
                            semanticError(
                                statement.location,
                                "void method cannot return a value");
                        }
                    } else {
                        if (!statement.expression) {
                            semanticError(
                                statement.location,
                                "non-void method must return a value");
                        }
                        const Type returnType = checkExpression(
                            *statement.expression,
                            scopes,
                            flow.initialized,
                            methods);
                        if (returnType != method.returnType) {
                            semanticError(
                                statement.expression->location,
                                "return expression must have the method's declared type");
                        }
                    }
                    flow.fallsThrough = false;
                    return flow;

                case StmtKind::Block:
                    return analyzeBlock(
                        statement.body, std::move(flow), scopes,
                        methods, method, nextSymbolId);

                case StmtKind::If: {
                    const Type conditionType = checkExpression(
                        *statement.condition, scopes, flow.initialized, methods);
                    if (conditionType != Type::Boolean) {
                        semanticError(
                            statement.condition->location,
                            "if condition must have type boolean");
                    }

                    Flow thenFlow = analyzeBlock(
                        statement.body, flow, scopes, methods, method, nextSymbolId);
                    Flow elseFlow = flow;
                    if (statement.hasElse) {
                        elseFlow = analyzeBlock(
                            statement.elseBody, std::move(elseFlow), scopes,
                            methods, method, nextSymbolId);
                    }

                    if (thenFlow.fallsThrough && elseFlow.fallsThrough) {
                        Initialized merged;
                        for (const std::size_t id : thenFlow.initialized) {
                            if (elseFlow.initialized.contains(id)) {
                                merged.insert(id);
                            }
                        }
                        flow.initialized = std::move(merged);
                        flow.fallsThrough = true;
                    } else if (thenFlow.fallsThrough) {
                        flow = std::move(thenFlow);
                    } else if (elseFlow.fallsThrough) {
                        flow = std::move(elseFlow);
                    } else {
                        flow.fallsThrough = false;
                    }
                    return flow;
                }
            }

            throw std::runtime_error("unknown statement kind");
        }

        Flow analyzeStatements(
            const std::vector<Stmt> &statements,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const Method &method,
            std::size_t &nextSymbolId) {
            for (const Stmt &statement : statements) {
                flow = analyzeStatement(
                    statement, std::move(flow), scopes, methods,
                    method, nextSymbolId);
            }
            return flow;
        }

        void checkMethod(const Method &method, const MethodTable &methods) {
            Scopes scopes(1);
            Flow flow;
            std::size_t nextSymbolId = 0;

            for (const Parameter &parameter : method.parameters) {
                if (scopes.back().contains(parameter.name)) {
                    semanticError(
                        parameter.location,
                        "duplicate parameter: " + parameter.name);
                }
                const std::size_t id = nextSymbolId++;
                scopes.back().emplace(
                    parameter.name, Symbol{parameter.type, id});
                flow.initialized.insert(id);
            }

            flow = analyzeStatements(
                method.body, std::move(flow), scopes, methods,
                method, nextSymbolId);

            if (method.returnType != Type::Void && flow.fallsThrough) {
                semanticError(
                    method.location,
                    method.returnType == Type::Int
                        ? "int method must return a value"
                        : "boolean method must return a value");
            }
        }
    }

    void SematicAnalyzer::analyze(const Program &program) const {
        MethodTable methods;

        for (const Method &method : program.methods) {
            if (method.name == "printInt") {
                semanticError(method.location, "method name 'printInt' is reserved");
            }
            if (!methods.emplace(method.name, &method).second) {
                semanticError(
                    method.location, "duplicate function: " + method.name);
            }
        }

        const auto entryPoint = methods.find("main");
        if (entryPoint == methods.end()) {
            throw std::runtime_error(
                "semantic error: missing required entry point 'main' "
                "(expected static void main())");
        }

        const Method &mainMethod = *entryPoint->second;
        if (mainMethod.returnType != Type::Void ||
            !mainMethod.parameters.empty()) {
            semanticError(
                mainMethod.location,
                "entry point 'main' must have signature 'static void main()'");
        }

        for (const Method &method : program.methods) {
            checkMethod(method, methods);
        }
    }
}
