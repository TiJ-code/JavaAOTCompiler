#pragma once

#include "jaot/parser.h"

namespace JAOT::IR {
    using LocalId = std::uint32_t;

    enum class ExprKind {
        Integer,
        Local,
        Binary,
        Call,
    };

    struct Expr {
        ExprKind kind;
        Type type;
        SourceLocation location {};

        std::int64_t integer = 0;
        LocalId local = 0;
        char binaryOperator = 0;
        std::string callee;
        std::vector<std::unique_ptr<Expr>> arguments;
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
    };

    struct Local {
        LocalId id;
        std::string name;
        Type type;
        bool isParameter;
    };

    enum class StatementKind {
        AssignLocal,
        Evaluate,
        Return,
    };

    struct Statement {
        StatementKind kind;
        SourceLocation location {};
        LocalId target = 0;
        std::unique_ptr<Expr> expression;
    };

    struct Method {
        std::string name;
        Type returnType;
        std::vector<LocalId> parameters;
        std::vector<Local> locals;
        std::vector<Statement> statements;
    };

    struct Program {
        std::vector<Method> methods;
    };
}