#include "jaot/semantic.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace JAOT {
    namespace {
        using MethodTable = std::unordered_map<std::string, MethodSignature>;
        using Initialized = std::unordered_set<std::size_t>;

        struct FieldInfo {
            TypeRef type;
        };

        using FieldTable =
            std::unordered_map<std::string, std::unordered_map<std::string, FieldInfo>>;

        struct Symbol {
            TypeRef type;
            std::size_t id;
        };

        using Scope = std::unordered_map<std::string, Symbol>;
        using Scopes = std::vector<Scope>;

        struct Flow {
            Initialized initialized;
            bool fallsThrough = true;
        };

        MethodSignature signatureOf(
            const std::string &ownerClass,
            const Method &method
        ) {
            MethodSignature signature;
            signature.name = method.name;
            signature.ownerClass = ownerClass;
            signature.returnType = method.returnType;
            for (const Parameter &parameter : method.parameters) {
                signature.parameterTypes.push_back(parameter.type);
            }
            return signature;
        }

        const MethodSignature *findMethod(
            const MethodTable &methods,
            const std::string &currentClass,
            const Expr &expression
        ) {
            const std::string &owner =
                expression.calleeClass.empty()
                    ? currentClass
                    : expression.calleeClass;
            const auto found = methods.find(owner + "." + expression.callee);
            return found == methods.end() ? nullptr : &found->second;
        }

        [[noreturn]] void semanticError(
            const SourceLocation location,
            const std::string &message
        ) {
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

        TypeRef checkExpression(
            const Expr &expression,
            const Scopes &scopes,
            const Initialized &initialized,
            const MethodTable &methods,
            const FieldTable &fields,
            const std::string &currentClass
        ) {
            switch (expression.kind) {
                case ExprKind::Integer:
                    return TypeRef{.kind = Type::Int, .className = {}};

                case ExprKind::Boolean:
                    return TypeRef{.kind = Type::Boolean, .className = {}};

                case ExprKind::Null:
                    return TypeRef{.kind = Type::Reference, .className = {}};

                case ExprKind::NewObject:
                    return TypeRef{.kind = Type::Reference, .className = expression.className};

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
                    const TypeRef receiverType = checkExpression(
                        *expression.left, scopes, initialized, methods, fields,
                        currentClass);
                    if (receiverType.kind != Type::Reference) {
                        semanticError(
                            expression.location,
                            "field access requires a reference receiver");
                    }
                    if (receiverType.className.empty()) {
                        semanticError(
                            expression.location,
                            "field access requires a class-typed reference");
                    }
                    const FieldInfo *field =
                        findField(fields, receiverType.className, expression.name);
                    if (field == nullptr) {
                        semanticError(
                            expression.location,
                            "unknown field: " + expression.name);
                    }
                    return field->type;
                }

                case ExprKind::Binary: {
                    const TypeRef left = checkExpression(
                        *expression.left, scopes, initialized, methods, fields,
                        currentClass);
                    const TypeRef right = checkExpression(
                        *expression.right, scopes, initialized, methods, fields,
                        currentClass);

                    if (expression.op == "+" || expression.op == "-" ||
                        expression.op == "*" || expression.op == "/") {
                        if (left.kind != Type::Int || right.kind != Type::Int) {
                            semanticError(
                                expression.location,
                                "arithmetic operands must have type int (left: " +
                                typeName(left) + ", right: " + typeName(right) + ")");
                        }
                        return TypeRef{Type::Int, {}};
                    }

                    if (expression.op == "<" || expression.op == "<=" ||
                        expression.op == ">" || expression.op == ">=") {
                        if (left.kind != Type::Int || right.kind != Type::Int) {
                            semanticError(
                                expression.location,
                                "relational operands must have type int (left: " +
                                typeName(left) + ", right: " + typeName(right) + ")");
                        }
                        return TypeRef{Type::Boolean, {}};
                    }

                    if (expression.op == "==" || expression.op == "!=") {
                        if (!isAssignable(left, right) &&
                            !isAssignable(right, left)) {
                                semanticError(
                                    expression.location,
                                    "equality operands must have the same reference type or be null (left: " +
                                    typeName(left) + ", right: " + typeName(right) + ")"
                                );
                        }

                        if (left.kind != right.kind ||
                            (left.kind != Type::Int &&
                             left.kind != Type::Boolean &&
                             left.kind != Type::Reference)) {
                            semanticError(
                                expression.location,
                                "equality operands must have the same value type (left: " +
                                typeName(left) + ", right: " + typeName(right) + ")");
                        }
                        return TypeRef{Type::Boolean, {}};
                    }

                    if (expression.op == "&&" || expression.op == "||") {
                        if (left.kind != Type::Boolean || right.kind != Type::Boolean) {
                            semanticError(
                                expression.location,
                                "logical operators require boolean operands (left: " +
                                typeName(left) + ", right: " + typeName(right) + ")");
                        }

                        return TypeRef{Type::Boolean, {}};
                    }

                    semanticError(expression.location, "unknown binary operator");
                }

                case ExprKind::Unary: {
                    const TypeRef operandType = checkExpression(
                        *expression.operand, scopes, initialized, methods, fields,
                        currentClass);

                    if (operandType.kind != Type::Boolean) {
                        semanticError(
                            expression.location,
                            "logical negation requires a boolean operand (actual: " +
                            typeName(operandType) + ")");
                    }

                    return TypeRef{Type::Boolean, {}};
                }

                case ExprKind::Call: {
                    if (expression.callee == "printInt") {
                        if (expression.arguments.size() != 1) {
                            semanticError(
                                expression.location,
                                "printInt expects one argument");
                        }
                        const TypeRef argumentType = checkExpression(
                                *expression.arguments[0],
                                scopes,
                                initialized,
                                methods,
                                fields,
                                currentClass);
                        if (argumentType.kind != Type::Int) {
                            semanticError(
                                expression.arguments[0]->location,
                                "printInt argument must have type int (actual: " +
                                typeName(argumentType) + ")");
                        }
                        return TypeRef{Type::Void, {}};
                    }

                    if (expression.callee == "printBool") {
                        if (expression.arguments.size() != 1) {
                            semanticError(
                                expression.location,
                                "printBool expects one argument"
                            );
                        }
                        const TypeRef argumentType = checkExpression(
                                *expression.arguments[0],
                                scopes,
                                initialized,
                                methods,
                                fields,
                                currentClass);
                        if (argumentType.kind != Type::Boolean) {
                            semanticError(
                                expression.arguments[0]->location,
                                "printBool arguments must have type boolean (actual: " +
                                typeName(argumentType) + ")");
                        }
                        return TypeRef{Type::Void, {}};
                    }

                    const MethodSignature *callee =
                        findMethod(methods, currentClass, expression);
                    if (callee == nullptr) {
                        semanticError(
                            expression.location,
                            "unknown function: " +
                            (expression.calleeClass.empty()
                                ? expression.callee
                                : expression.calleeClass + "." +
                                  expression.callee));
                    }

                    if (expression.arguments.size() !=
                        callee->parameterTypes.size()) {
                        semanticError(
                            expression.location,
                            "function " + expression.callee + " expects " +
                            std::to_string(callee->parameterTypes.size()) +
                            " arguments");
                    }

                    for (std::size_t i = 0; i < expression.arguments.size(); ++i) {
                        const Expr &argument = *expression.arguments[i];
                        const TypeRef &parameter = callee->parameterTypes[i];

                        const TypeRef argumentType = checkExpression(
                            argument, scopes, initialized, methods, fields,
                            currentClass
                        );

                        if (!isAssignable(argumentType, parameter)) {
                            semanticError(
                                expression.arguments[i]->location,
                                "argument " + std::to_string(i) +
                                " type does not match parameter (expected: " +
                                typeName(parameter) + ", actual: " +
                                typeName(argumentType) + ")");
                        }
                    }

                    return callee->returnType;
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
            std::size_t &nextSymbolId,
            const std::string &currentClass);

        Flow analyzeBlock(
            const std::vector<Stmt> &statements,
            Flow flow,
            Scopes &scopes,
            const MethodTable &methods,
            const FieldTable &fields,
            const Method &method,
            std::size_t &nextSymbolId,
            const std::string &currentClass) {
            scopes.emplace_back();
            flow = analyzeStatements(
                statements, std::move(flow), scopes, methods, fields,
                method, nextSymbolId, currentClass);

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
            std::size_t &nextSymbolId,
            const std::string &currentClass
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
                        const TypeRef initializerType = checkExpression(
                            *statement.expression,
                            scopes,
                            flow.initialized,
                            methods,
                            fields,
                            currentClass);

                        if (statement.declaredType.kind == Type::Reference) {
                            if (!isAssignable(initializerType, statement.declaredType)) {
                                semanticError(
                                    statement.expression->location,
                                    "reference initializer must have type '" +
                                    statement.declaredType.className +
                                    "' or null (actual: " +
                                    typeName(initializerType) + ")"
                                );
                            }
                        } else if (!sameType(initializerType, statement.declaredType)) {
                            semanticError(
                                statement.expression->location,
                                "variable initializer must have type " +
                                std::string(statement.declaredType.kind == Type::Int
                                    ? "int" : "boolean") +
                                " (actual: " + typeName(initializerType) + ")"
                            );
                        }
                    }

                    const std::size_t id = nextSymbolId++;
                    scopes.back().emplace(
                        statement.name, Symbol{
                            statement.declaredType,
                            id
                    });
                    if (statement.expression) {
                        flow.initialized.insert(id);
                    }
                    return flow;
                }

                case StmtKind::Assignment: {
                    const TypeRef valueType = checkExpression(
                        *statement.expression, scopes, flow.initialized,
                        methods, fields, currentClass);

                    if (statement.assignmentTarget) {
                        const TypeRef targetType = checkExpression(
                            *statement.assignmentTarget, scopes,
                            flow.initialized, methods, fields, currentClass);
                        if (targetType.kind == Type::Reference) {
                            if (!isAssignable(valueType, targetType)) {
                                semanticError(
                                    statement.expression->location,
                                    "reference assignment must match the field type or be null (expected: " +
                                    typeName(targetType) + ", actual: " +
                                    typeName(valueType) + ")");
                            }
                        } else if (!sameType(valueType, targetType)) {
                            semanticError(
                                statement.expression->location,
                                "assignment value must have the same type as its target (expected: " +
                                typeName(targetType) + ", actual: " +
                                typeName(valueType) + ")");
                        }
                        return flow;
                    }

                    const Symbol *symbol = findSymbol(scopes, statement.name);
                    if (symbol == nullptr) {
                        semanticError(
                            statement.location,
                            "unknown variable: " + statement.name);
                    }

                    if (symbol->type.kind == Type::Reference) {
                        if (!isAssignable(valueType, symbol->type)) {
                            semanticError(
                                statement.expression->location,
                                "reference assignment must have type '" +
                                symbol->type.className + "' or null (actual: " +
                                typeName(valueType) + ")"
                            );
                        }
                    } else if (!sameType(valueType, symbol->type)) {
                        semanticError(
                            statement.expression->location,
                            "assignment value must have the same type as its target (expected: " +
                            typeName(symbol->type) + ", actual: " +
                            typeName(valueType) + ")"
                        );
                    }

                    flow.initialized.insert(symbol->id);
                    return flow;
                }

                case StmtKind::Expression:
                    checkExpression(
                        *statement.expression, scopes, flow.initialized,
                        methods, fields, currentClass);
                    return flow;

                case StmtKind::Return:
                    if (method.returnType.kind == Type::Void) {
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
                        const TypeRef returnType = checkExpression(
                            *statement.expression,
                            scopes,
                            flow.initialized,
                            methods,
                            fields,
                            currentClass);
                        if (method.returnType.kind == Type::Reference) {
                            if (!isAssignable(returnType, method.returnType)) {
                                semanticError(
                                    statement.expression->location,
                                    "return expression must have type '" +
                                        method.returnType.className +
                                        "' or null (actual: " +
                                        typeName(returnType) + ")");
                            }
                        } else if (!sameType(returnType, method.returnType)) {
                            semanticError(
                                statement.expression->location,
                                "return expression must have the method's declared type (expected: " +
                                typeName(method.returnType) + ", actual: " +
                                typeName(returnType) + ")");
                        }
                    }
                    flow.fallsThrough = false;
                    return flow;

                case StmtKind::Block:
                    return analyzeBlock(
                        statement.body, std::move(flow), scopes,
                        methods, fields, method, nextSymbolId, currentClass);

                case StmtKind::If: {
                    const TypeRef conditionType = checkExpression(
                        *statement.condition, scopes, flow.initialized,
                        methods, fields, currentClass);
                    if (conditionType.kind != Type::Boolean) {
                        semanticError(
                            statement.condition->location,
                            "if condition must have type boolean (actual: " +
                            typeName(conditionType) + ")");
                    }

                    Flow thenFlow = analyzeBlock(
                        statement.body, flow, scopes, methods, fields,
                        method, nextSymbolId, currentClass);
                    Flow elseFlow = flow;
                    if (statement.hasElse) {
                        elseFlow = analyzeBlock(
                            statement.elseBody, std::move(elseFlow), scopes,
                            methods, fields, method, nextSymbolId,
                            currentClass);
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
                    const TypeRef conditionType = checkExpression(
                        *statement.condition, scopes, flow.initialized,
                        methods, fields, currentClass);

                    if (conditionType.kind != Type::Boolean) {
                        semanticError(
                            statement.condition->location,
                            "while condition must have type boolean (actual: " +
                            typeName(conditionType) + ")");
                    }

                    (void)analyzeBlock(
                        statement.body, flow, scopes, methods, fields,
                        method, nextSymbolId, currentClass);

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
            std::size_t &nextSymbolId,
            const std::string &currentClass) {
            for (const Stmt &statement : statements) {
                flow = analyzeStatement(
                    statement, std::move(flow), scopes, methods, fields,
                    method, nextSymbolId, currentClass);
            }
            return flow;
        }

        void checkMethod(
            const Method &method,
            const MethodTable &methods,
            const FieldTable &fields,
            const std::string &currentClass
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
                    parameter.name, Symbol{parameter.type, id});
                flow.initialized.insert(id);
            }

            flow = analyzeStatements(
                method.body, std::move(flow), scopes, methods, fields,
                method, nextSymbolId, currentClass);

            if (method.returnType.kind != Type::Void && flow.fallsThrough) {
                semanticError(
                    method.location,
                    method.returnType.kind == Type::Int
                        ? "int method must return a value"
                        : "boolean method must return a value");
            }
        }
    }

    struct SematicAnalyzer::AnalysisContext {
        std::unordered_set<std::string> classNames;
        FieldTable fields;
        MethodTable methods;
        const Method *mainMethod = nullptr;
    };

    void SematicAnalyzer::collectClassNames(
        const Program &program,
        AnalysisContext &context
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            if (!context.classNames.insert(declaration.name).second) {
                semanticError(
                    declaration.location,
                    "duplicate class: " + declaration.name
                );
            }
        }
    }

    void SematicAnalyzer::validateMethodTypes(
        const Program &program,
        const AnalysisContext &context
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                if (method.returnType.kind == Type::Reference &&
                    !context.classNames.contains(method.returnType.className)) {
                    semanticError(
                        method.location,
                        "unknown return type: " + method.returnType.className
                    );
                }

                for (const Parameter &parameter : method.parameters) {
                    if (parameter.type.kind == Type::Reference &&
                            !context.classNames.contains(parameter.type.className)) {
                        semanticError(
                            parameter.location,
                            "unknown parameter type: " + parameter.type.className
                        );
                    }
                }
            }
        }
    }

    void SematicAnalyzer::validateRequiredMainClass(
        const Program &program
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            if (declaration.name == "Main") {
                return;
            }
        }

        throw std::runtime_error(
            "semantic error: missing required class 'Main'"
        );
    }

    void SematicAnalyzer::collectFields(
        const Program &program,
        AnalysisContext &context
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            std::unordered_set<std::string> fieldNames;

            for (const Field &field : declaration.fields) {
                if (!fieldNames.insert(field.name).second) {
                    semanticError(
                        field.location,
                        "duplicate field: " + field.name
                    );
                }

                if (field.type.kind == Type::Reference &&
                    !context.classNames.contains(field.type.className)) {
                    semanticError(
                        field.location,
                        "unknown field: " + field.type.className
                    );
                }

                context.fields[declaration.name].emplace(
                    field.name,
                    FieldInfo{field.type});
            }
        }
    }

    void SematicAnalyzer::collectMethods(
        const Program &program,
        AnalysisContext &context
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                if (method.name == "printInt" || method.name == "printBool") {
                    semanticError(
                        method.location,
                        "method name '" + method.name + "' is reserved"
                    );
                }

                if (!context.methods.emplace(
                        declaration.name + "." + method.name,
                        signatureOf(declaration.name, method)).second) {
                    semanticError(
                        method.location,
                        "duplicate method: " + declaration.name + "." +
                        method.name
                    );
                }

                if (declaration.name == "Main" && method.name == "main") {
                    context.mainMethod = &method;
                }
            }
        }
    }

    void SematicAnalyzer::validateEntryPoint(
        const AnalysisContext &context
    ) {
        if (context.mainMethod == nullptr) {
            throw std::runtime_error(
                "semantic error: missing required entry point 'Main.main' (expected static void main())"
            );
        }

        if (context.mainMethod->returnType.kind != Type::Void ||
            !context.mainMethod->parameters.empty()) {
            semanticError(
                context.mainMethod->location,
                "entry point 'Main.main' must have signature 'static void main()'"
            );
        }
    }

    void SematicAnalyzer::analyzeMethodBodies(
        const Program &program,
        const AnalysisContext &context
    ) {
        for (const ClassDeclaration &declaration : program.classes) {
            for (const Method &method : declaration.methods) {
                checkMethod(
                    method, context.methods, context.fields,
                    declaration.name);
            }
        }
    }

    void SematicAnalyzer::analyze(const Program &program) {
        AnalysisContext context;

        collectClassNames(program, context);
        validateMethodTypes(program, context);
        validateRequiredMainClass(program);
        collectFields(program, context);
        collectMethods(program, context);
        validateEntryPoint(context);
        analyzeMethodBodies(program, context);
    }
}
