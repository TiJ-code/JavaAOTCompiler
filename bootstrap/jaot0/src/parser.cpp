#include "jaot/parser.h"

#include <stdexcept>
#include <utility>

namespace JAOT {
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

    ParsedType Parser::parseType(const bool allowVoid) {
        const Token &token = current();

        TypeRef type {
            .kind = Type::Int,
            .className = {},
            .elementType = nullptr
        };

        if (match(TokenKind::KwChar)) {
            type.kind = Type::Char;
        } else if (match(TokenKind::KwByte)) {
            type.kind = Type::Byte;
        } else if (match(TokenKind::KwShort)) {
            type.kind = Type::Short;
        } else if (match(TokenKind::KwInt)) {
            type.kind = Type::Int;
        } else if (match(TokenKind::KwLong)) {
            type.kind = Type::Long;
        } else if (match(TokenKind::KwBoolean)) {
            type.kind = Type::Boolean;
        } else if (check(TokenKind::KwVoid)) {
            if (!allowVoid) {
                error("'void' is only valid as a method return type");
            }

            match(TokenKind::KwVoid);
            type.kind = Type::Void;
        } else if (check(TokenKind::Identifier)) {
            const Token &classToken = consume(TokenKind::Identifier, "expected type");

            type.kind = Type::Reference;
            type.className = classToken.text;
        } else {
            error("expected type");
        }

        if (type.kind == Type::Void && check(TokenKind::LBracket)) {
            error("'void' cannot be used as an array element type");
        }

        /*
         * consume empty [] paris here.
         *
         * parsing for declarations:
         *  - int[] values;
         *  - int[][] matrix;
         *
         *  does not consume [10], because [10] belongs to
         *  `new int[10]` and is parsed by parsePrimary().
         */
        while (
            check(TokenKind::LBracket) &&
            index_ + 1 < tokens_.size() &&
            tokens_[index_ + 1].kind == TokenKind::RBracket
        ) {
            ++index_; // [
            ++index_; // ]

            type = TypeRef {
                .kind = Type::Array,
                .className = {},
                .elementType = std::make_shared<TypeRef>(std::move(type))
            };
        }

        return ParsedType {
            .type = std::move(type),
            .location = {.line = token.line, .column = token.column}
        };
    }

    Program Parser::parse() {
        Program program;

        if (check(TokenKind::EndOfFile)) {
            error("expected 'class'");
        }

        while (!check(TokenKind::EndOfFile)) {
            program.classes.push_back(parseClass());
        }

        consume(TokenKind::EndOfFile, "expected one of file");
        return program;
    }

    ClassDeclaration Parser::parseClass() {
        consume(TokenKind::KwClass, "expected 'class'");
        const Token &name = consume(TokenKind::Identifier, "expected class name");

        ClassDeclaration declaration;
        declaration.name = name.text;
        declaration.location = {.line = name.line, .column = name.column};

        consume(TokenKind::LBrace, "expected '{' after class name");

        while (!check(TokenKind::KwStatic) &&
               !check(TokenKind::RBrace) &&
               !check(TokenKind::EndOfFile)) {
            declaration.fields.push_back(parseField());
        }

        while (!check(TokenKind::RBrace) &&
               !check(TokenKind::EndOfFile)) {
            declaration.methods.push_back(parseMethod());
        }

        consume(TokenKind::RBrace, "expected '}' after class");
        return declaration;
    }

    Field Parser::parseField() {
        const ParsedType type = parseType(false);

        const Token &name = consume(TokenKind::Identifier, "expected field name");
        consume(TokenKind::Semicolon, "expected ';' after field declaration");

        return {
            .type = type.type,
            .name = name.text,
            .location = type.location
        };
    }

    Method Parser::parseMethod() {
        consume(TokenKind::KwStatic, "expected 'static'");

        const ParsedType returnType = parseType(true);

        const auto &name = consume(TokenKind::Identifier, "expected method name");

        consume(TokenKind::LParen, "expected '(' after method name");

        Method method;
        method.returnType = returnType.type;
        method.name = name.text;
        method.location = returnType.location;

        if (!check(TokenKind::RParen)) {
            while (true) {
                const ParsedType parameterType = parseType(false);

                const auto &parameter =
                        consume(TokenKind::Identifier, "expected parameter name");

                method.parameters.push_back({
                    .type = parameterType.type,
                    .name = parameter.text,
                    .location = parameterType.location
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

} // namespace JAOT
