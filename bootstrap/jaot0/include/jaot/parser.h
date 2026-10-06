#pragma once

#include "jaot/token.h"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace JAOT {
    enum class Type {
        Int,
        Void,
    };

    struct SourceLocation {
        std::size_t line;
        std::size_t column;
    };

    enum class ExprKind {
        Integer,
        Variable,
        Binary,
        Call,
    };

    struct Expr {
        ExprKind kind;
        SourceLocation location {};

        int64_t integer = 0;

        std::string name;

        char op = 0;

        std::string callee;

        std::vector<std::unique_ptr<Expr> > arguments;

        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
    };

    enum class StmtKind {
        VarDecl,
        Assignment,
        Expression,
        Return,
    };

    struct Stmt {
        StmtKind kind;
        SourceLocation location {};

        std::string name;
        std::unique_ptr<Expr> expression;
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

        Method parseMethod();

        Stmt parseStatement();

        Stmt parseVariableDeclaration();

        Stmt parseAssignment();

        Stmt parseReturn();

        std::unique_ptr<Expr> parseExpression();

        std::unique_ptr<Expr> parseTerm();

        std::unique_ptr<Expr> parseFactor();

        std::unique_ptr<Expr> parsePrimary();

        void error(const std::string &message) const;

        std::vector<Token> tokens_;

        std::size_t index_ = 0;
    };
} // namespace JAOT
