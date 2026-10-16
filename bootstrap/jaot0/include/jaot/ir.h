#pragma once

#include "jaot/types.h"

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

namespace JAOT::IR {
    using LocalId = std::uint32_t;
    using BlockId = std::uint32_t;

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
    };

    struct Statement {
        StatementKind kind;
        SourceLocation location {};
        LocalId target = 0;
        std::unique_ptr<Expr> expression;
    };

    enum class TerminatorKind {
        Jump,
        Branch,
        Return,
    };

    struct Terminator {
        TerminatorKind kind = TerminatorKind::Return;
        SourceLocation location {};

        BlockId target = 0;
        BlockId trueTarget = 0;
        BlockId falseTarget = 0;

        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> expression;
    };

    struct BasicBlock {
        BlockId id = 0;
        std::vector<Statement> statements;
        Terminator terminator;
    };

    struct Method {
        std::string name;
        Type returnType;
        std::vector<LocalId> parameters;
        std::vector<Local> locals;
        std::vector<BasicBlock> blocks;
    };

    struct Program {
        std::vector<Method> methods;
    };
}