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
        Null,
        NewObject,
        Variable,
        FieldAccess,
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
        std::string className;

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
        std::string declaredTypeName;
        std::unique_ptr<Expr> expression;
        std::unique_ptr<Expr> assignmentTarget;
        std::unique_ptr<Expr> condition;

        std::vector<Stmt> body;
        std::vector<Stmt> elseBody;
        bool hasElse = false;
    };

    struct Parameter {
        Type type;
        std::string className;
        std::string name;
        SourceLocation location {};
    };

    struct Field {
        std::string typeName;
        std::string name;
        SourceLocation location {};
    };

    struct Method {
        Type returnType;
        std::string returnClassName;
        std::string name;
        std::vector<Parameter> parameters;
        std::vector<Stmt> body;
        SourceLocation location {};
    };

    struct ClassDeclaration {
        std::string name;
        SourceLocation location {};
        std::vector<Field> fields;
        std::vector<Method> methods;
    };

    struct Program {
        std::vector<ClassDeclaration> classes;
    };

    struct ParsedType {
        Type type;
        std::string className;
        SourceLocation location {};
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

        ParsedType parseType(bool allowVoid);

        ClassDeclaration parseClass();

        Field parseField();

        Method parseMethod();

        std::vector<Stmt> parseBlock();

        Stmt parseStatement();

        Stmt parseVariableDeclaration(
            Type type,
            std::string declaredTypeName = {});

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
