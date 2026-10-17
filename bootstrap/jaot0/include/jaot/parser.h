#pragma once

#include "jaot/token.h"
#include "jaot/types.h"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace JAOT {
    enum class ExprKind {
        Integer,
        Boolean,
        Variable,
        Binary,
        Unary,
        Call,
    };

    struct Expr {
        ExprKind kind;
        SourceLocation location {};

        int64_t integer = 0;
        bool boolean = false;

        std::string name;
        std::string op;
        std::string callee;

        std::vector<std::unique_ptr<Expr> > arguments;

        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
        std::unique_ptr<Expr> operand;
    };

    enum class StmtKind {
        VarDecl,
        Assignment,
        Expression,
        Return,
        Block,
        If,
        While,
    };

    struct Stmt {
        StmtKind kind;
        SourceLocation location {};
        Type declaredType = Type::Int;

        std::string name;
        std::unique_ptr<Expr> expression;
        std::unique_ptr<Expr> condition;

        std::vector<Stmt> body;
        std::vector<Stmt> elseBody;
        bool hasElse = false;
    };

    struct Parameter {
        Type type;
        std::string name;
        SourceLocation location {};
    };

    struct Method {
        Type returnType;
        std::string name;
        std::vector<Parameter> parameters;
        std::vector<Stmt> body;
        SourceLocation location {};
    };

    struct Program {
        std::string className;

        std::vector<Method> methods;
    };

    class Parser {
    public:
        explicit Parser(std::vector<Token> tokens);

        Program parse();

    private:
        const Token &current() const;

        const Token &previous() const;

        bool check(TokenKind kind) const;

        bool match(TokenKind kind);

        const Token &consume(TokenKind kind, const char *message);

        Type parseValueType();

        Method parseMethod();

        std::vector<Stmt> parseBlock();

        Stmt parseStatement();

        Stmt parseVariableDeclaration(Type type);

        Stmt parseAssignment();

        Stmt parseReturn();

        Stmt parseIf();

        Stmt parseWhile();

        std::unique_ptr<Expr> parseExpression();

        std::unique_ptr<Expr> parseEquality();

        std::unique_ptr<Expr> parseComparison();

        std::unique_ptr<Expr> parseAddition();

        std::unique_ptr<Expr> parseMultiplication();

        std::unique_ptr<Expr> parseUnary();

        std::unique_ptr<Expr> parsePrimary();

        std::unique_ptr<Expr> parseLogicalOr();

        std::unique_ptr<Expr> parseLogicalAnd();

        std::unique_ptr<Expr> makeBinary(const Token &operatorToken, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);

        void error(const std::string &message) const;

        std::vector<Token> tokens_;

        std::size_t index_ = 0;
    };
} // namespace JAOT
