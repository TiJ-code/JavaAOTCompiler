#pragma once

#include "jaot/types.h"

#include <cstdint>
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace JAOT::IR {
    using LocalId = std::uint32_t;
    using BlockId = std::uint32_t;

    enum class ExprKind {
        Integer,
        Boolean,
        Null,
        NewObject,
        Local,
        FieldAccess,
        Unary,
        Binary,
        Call,
    };

    struct Expr {
        ExprKind kind;
        TypeRef type;
        SourceLocation location {};

        std::int64_t integer = 0;
        bool boolean = false;
        LocalId local = 0;
        std::string className;
        std::size_t fieldOffset = 0;

        std::string binaryOperator;
        std::string unaryOperator;

        MethodSignature callee;

        std::unique_ptr<Expr> receiver;

        std::vector<std::unique_ptr<Expr>> arguments;
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
        std::unique_ptr<Expr> operand;
    };

    struct Local {
        LocalId id;
        std::string name;
        TypeRef type;
        bool isParameter;
    };

    enum class StatementKind {
        AssignLocal,
        AssignField,
        Evaluate,
    };

    struct Statement {
        StatementKind kind;
        SourceLocation location {};
        LocalId target = 0;
        std::unique_ptr<Expr> targetExpression;
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
        MethodSignature signature;
        std::optional<LocalId> receiver;
        std::vector<LocalId> parameters;
        std::vector<Local> locals;
        std::vector<BasicBlock> blocks;
    };

    struct Field {
        std::string name;
        TypeRef type;
        std::size_t offset = 0;
        std::size_t size = 0;
    };

    struct Class {
        std::string name;
        std::vector<Field> fields;
        std::size_t size = 0;
    };

    struct Program {
        std::vector<Class> classes;
        std::vector<Method> methods;
    };
}