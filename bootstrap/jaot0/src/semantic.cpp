#include "jaot/semantic.h"

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace JAOT {
    namespace {
        using MethodTable = std::unordered_map<std::string, const Method *>;
        using Initialized = std::unordered_set<std::size_t>;

        struct FieldInfo {
            Type type;
            std::string className;
        };

        using FieldTable =
            std::unordered_map<std::string, std::unordered_map<std::string, FieldInfo>>;

        struct Symbol {
            Type type;
            std::size_t id;
            std::string className;
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

        const FieldInfo *findField(
            const FieldTable &fields,
            const std::string &className,
            const std::string &fieldName) {
            const auto classFields = fields.find(className);
            if (classFields == fields.end()) {
                return nullptr;
            }
            const auto found = classFields->second.find(fieldName);
            return found == classFields->second.end() ? nullptr : &found->second;
        }

        std::optional<std::string> referenceClassName(
            const Expr &expression,
            const Scopes &scopes,
            const MethodTable &methods,
            const FieldTable &fields
        ) {
            if (expression.kind == ExprKind::NewObject) {
                return expression.className;
            }

            if (expression.kind == ExprKind::Variable) {
                const Symbol *symbol = findSymbol(scopes, expression.name);
                if (symbol != nullptr && symbol->type == Type::Reference) {
                    return symbol->className;
                }
            }

            if (expression.kind == ExprKind::FieldAccess && expression.left) {
                const auto receiverClass =
                    referenceClassName(*expression.left, scopes, methods, fields);
                if (receiverClass) {
                    const FieldInfo *field =
                        findField(fields, *receiverClass, expression.name);
                    if (field != nullptr && field->type == Type::Reference) {
                        return field->className;
                    }
                }
            }

            if (expression.kind == ExprKind::Call) {
                const auto method = methods.find(expression.callee);
                if (method != methods.end() &&
                    method->second->returnType == Type::Reference) {
                    return method->second->returnClassName;
                }
            }

            return std::nullopt;
        }

        bool referenceAssignableTo(
            const Expr &expression,
            const Scopes &scopes,
            const MethodTable &methods,
            const FieldTable &fields,
            const std::string &targetClassName
        ) {
            if (expression.kind == ExprKind::Null) {
                return true;
            }

            const auto sourceClassName =
                referenceClassName(expression, scopes, methods, fields);
            return sourceClassName.has_value() && *sourceClassName == targetClassName;
        }

        Type checkExpression(
            const Expr &expression,
            const Scopes &scopes,
            const Initialized &initialized,
            const MethodTable &methods,
            const FieldTable &fields
        ) {
            switch (expression.kind) {
                case ExprKind::Integer:
                    return Type::Int;

                case ExprKind::Boolean:
                    return Type::Boolean;

                case ExprKind::Null:
                case ExprKind::NewObject:
                    return Type::Reference;

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

                case ExprKind::FieldAccess: {
                    if (!expression.left) {
                        semanticError(expression.location, "incomplete field access");
                    }
                    const Type receiverType = checkExpression(
                        *expression.left, scopes, initialized, methods, fields);
                    if (receiverType != Type::Reference) {
                        semanticError(
                            expression.location,
                            "field access requires a reference receiver");
                    }
                    const auto receiverClass =
                        referenceClassName(
                            *expression.left, scopes, methods, fields);
                    if (!receiverClass) {
                        semanticError(
                            expression.location,
                            "field access requires a class-typed reference");
                    }
                    const FieldInfo *field =
                        findField(fields, *receiverClass, expression.name);
                    if (field == nullptr) {
                        semanticError(
                            expression.location,
                            "unknown field: " + expression.name);
                    }
                    return field->type;
                }

                case ExprKind::Binary: {
                    const Type left = checkExpression(
                        *expression.left, scopes, initialized, methods, fields);
                    const Type right = checkExpression(
                        *expression.right, scopes, initialized, methods, fields);

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
                        if (left == Type::Reference) {
                            const auto leftClassName =
                                referenceClassName(
                                    *expression.left, scopes, methods, fields);
                            const auto rightClassName =
                                referenceClassName(
                                    *expression.right, scopes, methods, fields);

                            if (leftClassName.has_value() &&
                                rightClassName.has_value() &&
                                *leftClassName != *rightClassName) {
                                semanticError(
                                    expression.location,
                                    "equality operands must have the same reference type or be null"
                                );
                            }
                        }

                        if (left != right ||
                            (left != Type::Int &&
                             left != Type::Boolean &&
                             left != Type::Reference)) {
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
                    const Type operandType = checkExpression(
                        *expression.operand, scopes, initialized, methods, fields);

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
                                methods,
                                fields) != Type::Int) {
                            semanticError(
                                expression.arguments[0]->location,
                                "printInt argument must have type int");
                        }
                        return Type::Void;
                    }

                    if (expression.callee == "printBool") {
                        if (expression.arguments.size() != 1) {
                            semanticError(
                                expression.location,
                                "printBool expects one argument"
                            );
                        }
                        if (checkExpression(
                                *expression.arguments[0],
                                scopes,
                                initialized,
                                methods,
                                fields) != Type::Boolean) {
                            semanticError(
                                expression.arguments[0]->location,
                                "printBool arguments must have type boolean");
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
                        const Expr &argument = *expression.arguments[i];
                        const Parameter &parameter = callee.parameters[i];

                        const Type argumentType = checkExpression(
                            argument, scopes, initialized, methods, fields
                        );

                        const bool isReferenceAssignableTo = referenceAssignableTo(
                            argument, scopes, methods, fields,
                            parameter.className
                        );

                        if (argumentType != parameter.type ||
                            (parameter.type == Type::Reference && !isReferenceAssignableTo)) {
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
            const FieldTable &fields,
            const Method &method,
            std::size_t &nextSymbolId);

        Flow analyzeBlock(
            const std::vector<Stmt> &statements,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const FieldTable &fields,
            const Method &method,
            std::size_t &nextSymbolId) {
            scopes.emplace_back();
            flow = analyzeStatements(
                statements, std::move(flow), scopes, methods, fields,
                method, nextSymbolId);

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
            const FieldTable &fields,
            const Method &method,
            std::size_t &nextSymbolId
        ) {
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
                            methods,
                            fields);

                        if (statement.declaredType == Type::Reference) {
                            if (initializerType != Type::Reference ||
                                !referenceAssignableTo(
                                    *statement.expression, scopes, methods, fields,
                                    statement.declaredTypeName)) {
                                semanticError(
                                    statement.expression->location,
                                    "reference initializer must have type '" +
                                    statement.declaredTypeName + "' or null"
                                );
                            }
                        } else if (initializerType != statement.declaredType) {
                            semanticError(
                                statement.expression->location,
                                "variable initializer must have type " +
                                std::string(statement.declaredType == Type::Int
                                    ? "int" : "boolean")
                            );
                        }
                    }

                    const std::size_t id = nextSymbolId++;
                    scopes.back().emplace(
                        statement.name, Symbol{
                            statement.declaredType,
                            id,
                            statement.declaredTypeName
                    });
                    if (statement.expression) {
                        flow.initialized.insert(id);
                    }
                    return flow;
                }

                case StmtKind::Assignment: {
                    const Type valueType = checkExpression(
                        *statement.expression, scopes, flow.initialized,
                        methods, fields);

                    if (statement.assignmentTarget) {
                        const Type targetType = checkExpression(
                            *statement.assignmentTarget, scopes,
                            flow.initialized, methods, fields);
                        if (targetType == Type::Reference) {
                            const auto targetClass = referenceClassName(
                                *statement.assignmentTarget, scopes, methods, fields);
                            if (!targetClass || valueType != Type::Reference ||
                                !referenceAssignableTo(
                                    *statement.expression, scopes, methods, fields,
                                    *targetClass)) {
                                semanticError(
                                    statement.expression->location,
                                    "reference assignment must match the field type or be null");
                            }
                        } else if (valueType != targetType) {
                            semanticError(
                                statement.expression->location,
                                "assignment value must have the same type as its target");
                        }
                        return flow;
                    }

                    const Symbol *symbol = findSymbol(scopes, statement.name);
                    if (symbol == nullptr) {
                        semanticError(
                            statement.location,
                            "unknown variable: " + statement.name);
                    }

                    if (symbol->type == Type::Reference) {
                        if (valueType != Type::Reference ||
                            !referenceAssignableTo(
                                *statement.expression, scopes, methods, fields,
                                symbol->className)) {
                            semanticError(
                                statement.expression->location,
                                "reference assignment must have type '" +
                                symbol->className + "' or null"
                            );
                        }
                    } else if (valueType != symbol->type) {
                        semanticError(
                            statement.expression->location,
                            "assignment value must have the same type as its target"
                        );
                    }

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
                        *statement.expression, scopes, flow.initialized,
                        methods, fields);
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
                            methods,
                            fields);
                        if (method.returnType == Type::Reference) {
                            if (returnType != Type::Reference ||
                                !referenceAssignableTo(
                                    *statement.expression,
                                    scopes,
                                    methods,
                                    fields,
                                    method.returnClassName)) {
                                semanticError(
                                    statement.expression->location,
                                    "return expression must have type '" +
                                        method.returnClassName + "' or null");
                            }
                        } else if (returnType != method.returnType) {
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
                        methods, fields, method, nextSymbolId);

                case StmtKind::If: {
                    const Type conditionType = checkExpression(
                        *statement.condition, scopes, flow.initialized,
                        methods, fields);
                    if (conditionType != Type::Boolean) {
                        semanticError(
                            statement.condition->location,
                            "if condition must have type boolean");
                    }

                    Flow thenFlow = analyzeBlock(
                        statement.body, flow, scopes, methods, fields,
                        method, nextSymbolId);
                    Flow elseFlow = flow;
                    if (statement.hasElse) {
                        elseFlow = analyzeBlock(
                            statement.elseBody, std::move(elseFlow), scopes,
                            methods, fields, method, nextSymbolId);
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

                case StmtKind::While: {
                    const Type conditionType = checkExpression(
                        *statement.condition, scopes, flow.initialized,
                        methods, fields);

                    if (conditionType != Type::Boolean) {
                        semanticError(
                            statement.condition->location,
                            "while condition must have type boolean");
                    }

                    (void)analyzeBlock(
                        statement.body, flow, scopes, methods, fields,
                        method, nextSymbolId);

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
            const FieldTable &fields,
            const Method &method,
            std::size_t &nextSymbolId) {
            for (const Stmt &statement : statements) {
                flow = analyzeStatement(
                    statement, std::move(flow), scopes, methods, fields,
                    method, nextSymbolId);
            }
            return flow;
        }

        void checkMethod(
            const Method &method,
            const MethodTable &methods,
            const FieldTable &fields
        ) {
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
                    parameter.name, Symbol{parameter.type, id, parameter.className});
                flow.initialized.insert(id);
            }

            flow = analyzeStatements(
                method.body, std::move(flow), scopes, methods, fields,
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
        std::unordered_set<std::string> classNames;

        FieldTable fields;
        for (const ClassDeclaration &declaration : program.classes) {
            if (!classNames.insert(declaration.name).second) {
                semanticError(
                    declaration.location,
                    "duplicate class: " + declaration.name
                );
            }
        }

        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                if (method.returnType == Type::Reference &&
                    !classNames.contains(method.returnClassName)) {
                    semanticError(
                        method.location,
                        "unknown return type: " + method.returnClassName
                    );
                }

                for (const Parameter &parameter : method.parameters) {
                    if (parameter.type == Type::Reference &&
                        !classNames.contains(parameter.className)) {
                        semanticError(
                            parameter.location,
                            "unknown parameter type: " + parameter.className
                        );
                    }
                }
            }
        }

        const ClassDeclaration *mainClass = nullptr;
        for (const ClassDeclaration &declaration : program.classes) {
            if (declaration.name == "Main") {
                mainClass = &declaration;
                break;
            }
        }

        if (mainClass == nullptr) {
            throw std::runtime_error(
                "semantic error: missing required class 'Main'"
            );
        }

        for (const ClassDeclaration &declaration : program.classes) {
            std::unordered_set<std::string> fieldNames;

            for (const Field &field : declaration.fields) {
                if (!fieldNames.insert(field.name).second) {
                    semanticError(
                        field.location,
                        "duplicate field: " + field.name
                    );
                }

                if (field.typeName != "int" &&
                    field.typeName != "boolean" &&
                    !classNames.contains(field.typeName)) {
                    semanticError(
                        field.location,
                        "unknown field: " + field.typeName
                    );
                }

                const Type fieldType = field.typeName == "int"
                    ? Type::Int
                    : field.typeName == "boolean" ? Type::Boolean : Type::Reference;
                fields[declaration.name].emplace(
                    field.name,
                    FieldInfo{
                        fieldType,
                        fieldType == Type::Reference ? field.typeName : ""
                    });
            }
        }

        MethodTable methods;
        const Method *mainMethod = nullptr;

        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                if (method.name == "printInt" || method.name == "printBool") {
                    semanticError(
                        method.location,
                        "method name '" + method.name + "' is reserved"
                    );
                }

                if (!methods.emplace(method.name, &method).second) {
                    semanticError(
                        method.location,
                        "duplicate function: " + method.name
                    );
                }

                if (declaration.name == "Main" && method.name == "main") {
                    mainMethod = &method;
                }
            }
        }

        if (mainMethod == nullptr) {
            throw std::runtime_error(
                "semantic error: missing required entry point 'Main.main' (expected static void main())"
            );
        }

        if (mainMethod->returnType != Type::Void ||
            !mainMethod->parameters.empty()) {
            semanticError(
                mainMethod->location,
                "entry point 'Main.main' must have signature 'static void main()'"
            );
        }

        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                checkMethod(method, methods, fields);
            }
        }
    }
}
