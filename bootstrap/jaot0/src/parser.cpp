#include "jaot/parser.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace JAOT {
    namespace {
        int64_t parseIntegerLiteral(const Token &token, bool negative = false) {
            try {
                const unsigned long long magnitude = std::stoull(token.text);

                const auto maxPositive = static_cast<unsigned long long>(
                    std::numeric_limits<int32_t>::max()
                );
                const auto maxNegativeMagnitude = maxPositive + 1;

                if (magnitude > (negative ? maxNegativeMagnitude : maxPositive)) {
                    throw std::out_of_range("integer literal out of range");
                }

                const int64_t value = static_cast<int64_t>(magnitude);
                return negative ? -value : value;
            } catch (const std::out_of_range &) {
                throw std::runtime_error(
                    "parser error at " + std::to_string(token.line) + ":" +
                    std::to_string(token.column) + ": integer literal out of 32-bit range"
                );
            }
        }
    }

    Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
    }

    const Token &Parser::current() const { return tokens_[index_]; }

    const Token &Parser::previous() const { return tokens_[index_ - 1]; }

    bool Parser::check(TokenKind kind) const { return current().kind == kind; }

    bool Parser::match(TokenKind kind) {
        if (!check(kind)) {
            return false;
        }

        ++index_;
        return true;
    }

    const Token &Parser::consume(TokenKind kind, const char *message) {
        if (!check(kind)) {
            error(message);
        }

        return tokens_[index_++];
    }

    void Parser::error(const std::string &message) const {
        const auto &token = current();

        throw std::runtime_error("parser error at " + std::to_string(token.line) +
                                 ":" + std::to_string(token.column) + ": " + message);
    }

    Type Parser::parseValueType() {
        if (match(TokenKind::KwInt)) {
            return Type::Int;
        }
        if (match(TokenKind::KwBoolean)) {
            return Type::Boolean;
        }
        error("expected 'int' or 'boolean'");
    }

    Program Parser::parse() {
        consume(TokenKind::KwClass, "expected 'class'");

        const auto &className = consume(TokenKind::Identifier, "expected class name");

        Program program;
        program.className = className.text;

        consume(TokenKind::LBrace, "expected '{' after class name");

        while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
            program.methods.push_back(parseMethod());
        }

        consume(TokenKind::RBrace, "expected '}' after class");

        consume(TokenKind::EndOfFile, "expected end of file");

        return program;
    }

    Method Parser::parseMethod() {
        consume(TokenKind::KwStatic, "expected 'static'");

        const auto &returnTypeToken = current();
        Type returnType;

        if (match(TokenKind::KwVoid)) {
            returnType = Type::Void;
        } else if (match(TokenKind::KwInt)) {
            returnType = Type::Int;
        } else if (match(TokenKind::KwBoolean)) {
            returnType = Type::Boolean;
        } else {
            error("unexpected method return type");
        }

        const auto &name = consume(TokenKind::Identifier, "expected method name");

        consume(TokenKind::LParen, "expected '(' after method name");

        Method method;
        method.returnType = returnType;
        method.name = name.text;
        method.location = { .line = returnTypeToken.line, .column = returnTypeToken.column };

        if (!check(TokenKind::RParen)) {
            while (true) {
                const Type parameterType = parseValueType();
                const auto &parameter =
                        consume(TokenKind::Identifier, "expected parameter name");

                method.parameters.push_back({
                    .type = parameterType,
                    .name = parameter.text,
                    .location = {
                        .line = parameter.line,
                        .column = parameter.column
                    }
                });

                if (!match(TokenKind::Comma)) {
                    break;
                }
            }
        }

        consume(TokenKind::RParen, "expected ')' after parameters");

        consume(TokenKind::LBrace, "expected '{' before method body");

        while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
            method.body.push_back(parseStatement());
        }

        consume(TokenKind::RBrace, "expected '}' after method body");

        return method;
    }

    std::vector<Stmt> Parser::parseBlock() {
        consume(TokenKind::LBrace, "expected '{' before block");

        std::vector<Stmt> statements;

        while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
            statements.push_back(parseStatement());
        }

        consume(TokenKind::RBrace, "expected '}' after block");
        return statements;
    }

    Stmt Parser::parseStatement() {
        if (match(TokenKind::KwInt)) {
            return parseVariableDeclaration(Type::Int);
        }

        if (match(TokenKind::KwBoolean)) {
            return parseVariableDeclaration(Type::Boolean);
        }

        if (match(TokenKind::KwReturn)) {
            return parseReturn();
        }

        if (match(TokenKind::KwIf)) {
            return parseIf();
        }

        if (check(TokenKind::LBrace)) {
            Stmt statement;
            statement.kind = StmtKind::Block;
            statement.location = { current().line, current().column };
            statement.body = parseBlock();
            return statement;
        }

        if (check(TokenKind::Identifier) &&
            index_ + 1 < tokens_.size() &&
            tokens_[index_ + 1].kind == TokenKind::Equal) {
            return parseAssignment();
        }

        Stmt statement;
        statement.kind = StmtKind::Expression;
        statement.expression = parseExpression();
        statement.location = statement.expression->location;

        consume(TokenKind::Semicolon, "expected ';' after expression");

        return statement;
    }

    Stmt Parser::parseVariableDeclaration(Type type) {
        const auto &name = consume(TokenKind::Identifier, "expected variable name");

        Stmt statement;
        statement.kind = StmtKind::VarDecl;
        statement.declaredType = type;
        statement.name = name.text;
        statement.location = { .line = name.line, .column =  name.column };

        if (match(TokenKind::Equal)) {
            statement.expression = parseExpression();
        }

        consume(TokenKind::Semicolon, "expected ';' after variable declaration");

        return statement;
    }

    Stmt Parser::parseAssignment() {
        const auto &name = consume(TokenKind::Identifier, "expected variable name in assignment");

        consume(TokenKind::Equal, "expected '=' in assignment");

        Stmt statement;
        statement.kind = StmtKind::Assignment;
        statement.name = name.text;
        statement.location = {
            .line = name.line,
            .column = name.column
        };
        statement.expression = parseExpression();

        consume(TokenKind::Semicolon, "expected ';' after assignment");

        return statement;
    }

    Stmt Parser::parseReturn() {
        const auto &returnToken = previous();

        Stmt statement;
        statement.kind = StmtKind::Return;
        statement.location = { .line = returnToken.line, .column = returnToken.column };

        if (!check(TokenKind::Semicolon)) {
            statement.expression = parseExpression();
        }

        consume(TokenKind::Semicolon, "expected ';' after return");

        return statement;
    }

    Stmt Parser::parseIf() {
        const auto &ifToken = previous();

        Stmt statement;
        statement.kind = StmtKind::If;
        statement.location = {ifToken.line, ifToken.column};

        consume(TokenKind::LParen, "expected '(' after 'if'");
        statement.condition = parseExpression();
        consume(TokenKind::RParen, "expected ')' after condition");
        statement.body = parseBlock();

        if (match(TokenKind::KwElse)) {
            statement.hasElse = true;
            statement.elseBody = parseBlock();
        }

        return statement;
    }

    std::unique_ptr<Expr> Parser::makeBinary(
        const Token &operatorToken,
        std::unique_ptr<Expr> left,
        std::unique_ptr<Expr> right) {
        auto expression = std::make_unique<Expr>();
        expression->kind = ExprKind::Binary;
        expression->op = operatorToken.text;
        expression->location = {operatorToken.line, operatorToken.column};
        expression->left = std::move(left);
        expression->right = std::move(right);
        return expression;
    }

    std::unique_ptr<Expr> Parser::parseExpression() {
        return parseEquality();
    }

    std::unique_ptr<Expr> Parser::parseEquality() {
        auto left = parseComparison();
        while (check(TokenKind::EqualEqual) || check(TokenKind::BangEqual)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseComparison());
        }
        return left;
    }

    std::unique_ptr<Expr> Parser::parseComparison() {
        auto left = parseAddition();
        while (check(TokenKind::Less) ||
                check(TokenKind::LessEqual) ||
                check(TokenKind::Greater) ||
                check(TokenKind::GreaterEqual)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseAddition());
        }
        return left;
    }

    std::unique_ptr<Expr> Parser::parseAddition() {
        auto left = parseMultiplication();
        while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseMultiplication());
        }
        return left;
    }

    std::unique_ptr<Expr> Parser::parseMultiplication() {
        auto left = parseUnary();
        while (check(TokenKind::Star) || check(TokenKind::Slash)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseUnary());
        }
        return left;
    }

    std::unique_ptr<Expr> Parser::parseUnary() {
        if (match(TokenKind::Minus)) {
            const Token minusToken = current();

            if (match(TokenKind::Integer)) {
                auto expression = std::make_unique<Expr>();
                expression->kind = ExprKind::Integer;
                expression->integer = parseIntegerLiteral(previous(), true);
                expression->location = {minusToken.line, minusToken.column};
                return expression;
            }

            auto zero = std::make_unique<Expr>();
            zero->kind = ExprKind::Integer;
            zero->integer = 0;
            zero->location = {minusToken.line, minusToken.column};

            return makeBinary(minusToken, std::move(zero), parseUnary());
        }

        return parsePrimary();
    }

    std::unique_ptr<Expr> Parser::parsePrimary() {
        if (match(TokenKind::Integer)) {
            const auto &token = previous();
            auto expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Integer;
            expression->integer = parseIntegerLiteral(token);
            expression->location = {token.line, token.column};
            return expression;
        }

        if (match(TokenKind::KwTrue) || match(TokenKind::KwFalse)) {
            const auto &token = previous();
            auto expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Boolean;
            expression->boolean = token.kind == TokenKind::KwTrue;
            expression->location = {token.line, token.column};
            return expression;
        }

        if (match(TokenKind::Identifier)) {
            const Token identifier = previous();
            if (!match(TokenKind::LParen)) {
                auto expression = std::make_unique<Expr>();
                expression->kind = ExprKind::Variable;
                expression->name = identifier.text;
                expression->location = {identifier.line, identifier.column};
                return expression;
            }

            auto expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Call;
            expression->callee = identifier.text;
            expression->location = {identifier.line, identifier.column};

            if (!check(TokenKind::RParen)) {
                while (true) {
                    expression->arguments.push_back(parseExpression());
                    if (!match(TokenKind::Comma)) {
                        break;
                    }
                }
            }

            consume(TokenKind::RParen, "expected ')' after arguments");
            return expression;
        }

        if (match(TokenKind::LParen)) {
            auto expression = parseExpression();
            consume(TokenKind::RParen, "expected ')' after expression");
            return expression;
        }

        error("expected expression");
    }
} // namespace JAOT
