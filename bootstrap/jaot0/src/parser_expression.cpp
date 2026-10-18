#include "jaot/parser.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace JAOT {
    namespace {
        int64_t parseIntegerLiteral(const Token &token, const bool negative = false) {
            try {
                const unsigned long long magnitude = std::stoull(token.text);
                const auto maxPositive = static_cast<unsigned long long>(
                    std::numeric_limits<int32_t>::max());
                const auto maxNegativeMagnitude = maxPositive + 1;

                if (magnitude > (negative ? maxNegativeMagnitude : maxPositive)) {
                    throw std::out_of_range("integer literal out of range");
                }

                const int64_t value = static_cast<int64_t>(magnitude);
                return negative ? -value : value;
            } catch (const std::out_of_range &) {
                throw std::runtime_error(
                    "parser error at " + std::to_string(token.line) + ":" +
                    std::to_string(token.column) +
                    ": integer literal out of 32-bit range");
            }
        }
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
        return parseLogicalOr();
    }

    std::unique_ptr<Expr> Parser::parseLogicalOr() {
        auto left = parseLogicalAnd();
        while (check(TokenKind::Or)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseLogicalAnd());
        }
        return left;
    }

    std::unique_ptr<Expr> Parser::parseLogicalAnd() {
        auto left = parseEquality();
        while (check(TokenKind::And)) {
            const Token operatorToken = current();
            ++index_;
            left = makeBinary(operatorToken, std::move(left), parseEquality());
        }
        return left;
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
        if (match(TokenKind::Bang)) {
            const Token bangToken = previous();

            auto expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Unary;
            expression->op = "!";
            expression->operand = parseUnary();
            expression->location = {
                .line = bangToken.line,
                .column = !bangToken.column
            };
            return expression;
        }

        if (match(TokenKind::Minus)) {
            const Token minusToken = current();

            if (match(TokenKind::Integer)) {
                auto expression = std::make_unique<Expr>();
                expression->kind = ExprKind::Integer;
                expression->integer = parseIntegerLiteral(previous(), true);
                expression->location = {
                    .line = minusToken.line,
                    .column = minusToken.column
                };
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
        std::unique_ptr<Expr> expression;

        if (match(TokenKind::Integer)) {
            const Token &token = previous();

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Integer;
            expression->integer = parseIntegerLiteral(token);
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::KwTrue) || match(TokenKind::KwFalse)) {
            const Token &token = previous();

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Boolean;
            expression->boolean = token.kind == TokenKind::KwTrue;
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::KwNull)) {
            const Token &token = previous();

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Null;
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::KwNew)) {
            const Token &newToken = previous();
            const Token &className =
                consume(TokenKind::Identifier, "expected class name after 'new'");
            consume(TokenKind::LParen, "expected '(' after class name");
            consume(TokenKind::RParen, "expected ')' after constructor arguments");

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::NewObject;
            expression->className = className.text;
            expression->location = {newToken.line, newToken.column};
        } else if (match(TokenKind::Identifier)) {
            const Token identifier = previous();

            if (match(TokenKind::LParen)) {
                expression = std::make_unique<Expr>();
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
            } else {
                expression = std::make_unique<Expr>();
                expression->kind = ExprKind::Variable;
                expression->name = identifier.text;
                expression->location = {identifier.line, identifier.column};
            }
        } else if (match(TokenKind::LParen)) {
            expression = parseExpression();
            consume(TokenKind::RParen, "expected ')' after expression");
        } else {
            error("expected expression");
        }

        while (match(TokenKind::Dot)) {
            const Token field =
                consume(TokenKind::Identifier, "expected field name after '.'");

            auto access = std::make_unique<Expr>();
            access->kind = ExprKind::FieldAccess;
            access->name = field.text;
            access->location = {field.line, field.column};
            access->left = std::move(expression);
            expression = std::move(access);
        }

        return expression;
    }
} // namespace JAOT
