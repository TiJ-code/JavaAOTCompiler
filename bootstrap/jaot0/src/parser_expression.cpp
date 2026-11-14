#include "jaot/parser.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace JAOT {
    namespace {
        int64_t parseLiteral(const Token &token, const bool negative = false) {
            const bool isShort = token.kind == TokenKind::ShortInteger;
            const bool isLong = token.kind == TokenKind::LongInteger;

            try {
                std::string digits = token.text;

                if (isShort || isLong) {
                    digits.pop_back();
                }

                const unsigned long long magnitude = std::stoull(digits);
                const auto maxPositive = isShort
                    ? static_cast<unsigned long long>(
                        std::numeric_limits<int16_t>::max())
                    : isLong
                    ? static_cast<unsigned long long>(
                        std::numeric_limits<int64_t>::max())
                    : static_cast<unsigned long long>(
                        std::numeric_limits<int32_t>::max());
                const auto maxNegativeMagnitude = maxPositive + 1;

                if (magnitude > (negative ? maxNegativeMagnitude : maxPositive)) {
                    throw std::out_of_range("integer literal out of range");
                }

                if (negative &&
                    (isShort || isLong) &&
                    magnitude == maxNegativeMagnitude) {
                    return isShort
                        ? std::numeric_limits<int16_t>::min()
                        : std::numeric_limits<int64_t>::min();
                }

                const auto value = static_cast<int64_t>(magnitude);
                return negative ? -value : value;
            } catch (const std::exception &) {
                throw std::runtime_error(
                    "parser error at " + std::to_string(token.line) + ":" +
                    std::to_string(token.column) +
                    (isShort
                        ? ": short integer literal out of 16-bit range"
                        : isLong
                        ? ": long integer literal out of 64-bit range"
                        : ": integer literal out of 32-bit range"));
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
            const Token minusToken = previous();

            if (check(TokenKind::Integer) ||
                check(TokenKind::ShortInteger) ||
                check(TokenKind::LongInteger)) {
                const Token token = current();
                ++index_;

                auto expression = std::make_unique<Expr>();
                expression->kind = token.kind == TokenKind::ShortInteger
                    ? ExprKind::ShortInteger
                    : token.kind == TokenKind::LongInteger
                    ? ExprKind::LongInteger
                    : ExprKind::Integer;
                expression->integer = parseLiteral(token, true);
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
            expression->integer = parseLiteral(token);
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::ShortInteger)) {
            const Token &token = previous();
            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::ShortInteger;
            expression->integer = parseLiteral(token);
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::LongInteger)) {
            const Token &token = previous();

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::LongInteger;
            expression->integer = parseLiteral(token);
            expression->location = {token.line, token.column};
        } else if (match(TokenKind::Character)) {
            const Token &token = previous();

            expression = std::make_unique<Expr>();
            expression->kind = ExprKind::Character;
            expression->integer = static_cast<unsigned char>(token.text[0]);
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
            } else if (match(TokenKind::Dot)) {
                const Token method =
                    consume(TokenKind::Identifier, "expected method name after class name");

                if (match(TokenKind::LParen)) {
                    expression = std::make_unique<Expr>();
                    expression->kind = ExprKind::Call;
                    expression->callee = method.text;
                    expression->calleeClass = identifier.text;
                    expression->location = {
                        identifier.line, identifier.column};

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
                    auto receiver = std::make_unique<Expr>();
                    receiver->kind = ExprKind::Variable;
                    receiver->name = identifier.text;
                    receiver->location = {
                        identifier.line, identifier.column};

                    expression = std::make_unique<Expr>();
                    expression->kind = ExprKind::FieldAccess;
                    expression->name = method.text;
                    expression->location = {method.line, method.column};
                    expression->left = std::move(receiver);
                }
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
