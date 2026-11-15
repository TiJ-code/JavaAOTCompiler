#include "jaot/parser.h"

#include <utility>

namespace JAOT {
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
        if (check(TokenKind::KwChar) ||
            check(TokenKind::KwByte) ||
            check(TokenKind::KwInt) ||
            check(TokenKind::KwShort) ||
            check(TokenKind::KwLong) ||
            check(TokenKind::KwBoolean)) {
            const ParsedType type = parseType(false);
            return parseVariableDeclaration(type.type);
        }

        if (match(TokenKind::KwReturn)) {
            return parseReturn();
        }

        if (match(TokenKind::KwIf)) {
            return parseIf();
        }

        if (match(TokenKind::KwWhile)) {
            return parseWhile();
        }

        if (match(TokenKind::KwBreak)) {
            return parseBreak();
        }

        if (match(TokenKind::KwContinue)) {
            return parseContinue();
        }

        if (check(TokenKind::LBrace)) {
            Stmt statement;
            statement.kind = StmtKind::Block;
            statement.location = { current().line, current().column };
            statement.body = parseBlock();
            return statement;
        }

        if (check(TokenKind::Identifier)) {
            std::size_t lookahead = index_ + 1;

            while (lookahead < tokens_.size()) {
                if (tokens_[lookahead].kind == TokenKind::Dot &&
                    lookahead + 1 < tokens_.size() &&
                    tokens_[lookahead + 1].kind == TokenKind::Identifier) {
                    lookahead += 2;
                    continue;
                }

                if (tokens_[lookahead].kind == TokenKind::LBracket) {
                    std::size_t depth = 1;
                    ++lookahead;
                    while (lookahead < tokens_.size() && depth != 0) {
                        if (tokens_[lookahead].kind == TokenKind::LBracket) {
                            ++depth;
                        } else if (tokens_[lookahead].kind == TokenKind::RBracket) {
                            --depth;
                        }
                        ++lookahead;
                    }
                    continue;
                }

                break;
            }

            if (lookahead + 1 < tokens_.size() &&
                tokens_[lookahead].kind == TokenKind::Equal) {
                return parseAssignment();
            }
        }

        if (check(TokenKind::Identifier) &&
            index_ + 1 < tokens_.size() &&
            tokens_[index_ + 1].kind == TokenKind::Identifier) {
            const ParsedType type = parseType(false);
            return parseVariableDeclaration(type.type);
        }

        Stmt statement;
        statement.kind = StmtKind::Expression;
        statement.expression = parseExpression();
        statement.location = statement.expression->location;

        consume(TokenKind::Semicolon, "expected ';' after expression");

        return statement;
    }

    Stmt Parser::parseVariableDeclaration(
        TypeRef type
    ) {
        const auto &name = consume(TokenKind::Identifier, "expected variable name");

        Stmt statement;
        statement.kind = StmtKind::VarDecl;
        statement.declaredType = std::move(type);
        statement.name = name.text;
        statement.location = { .line = name.line, .column = name.column };

        if (match(TokenKind::Equal)) {
            statement.expression = parseExpression();
        }

        consume(TokenKind::Semicolon, "expected ';' after variable declaration");

        return statement;
    }

    Stmt Parser::parseAssignment() {
        const Token identifier =
            consume(TokenKind::Identifier, "expected variable name in assignment");

        auto target = std::make_unique<Expr>();
        target->kind = ExprKind::Variable;
        target->name = identifier.text;
        target->location = { .line = identifier.line, .column = identifier.column };

        while (true) {
            if (match(TokenKind::Dot)) {
                const Token field =
                    consume(TokenKind::Identifier, "expected field name after '.'");

                auto access = std::make_unique<Expr>();
                access->kind = ExprKind::FieldAccess;
                access->name = field.text;
                access->location = { .line = field.line, .column = identifier.column };
                access->left = std::move(target);
                target = std::move(access);
                continue;
            }

            if (match(TokenKind::LBracket)) {
                auto index = parseExpression();
                const Token bracket = consume(
                    TokenKind::RBracket,
                    "expected ']' after array index"
                );

                auto access = std::make_unique<Expr>();
                access->kind = ExprKind::ArrayAccess;
                access->location = { .line = bracket.line, .column = bracket.column };
                access->left = std::move(target);
                access->right = std::move(index);
                target = std::move(access);
                continue;
            }

            break;
        }

        Stmt statement;
        statement.kind = StmtKind::Assignment;
        statement.location = {
            .line = identifier.line,
            .column = identifier.column
        };

        if (target->kind == ExprKind::Variable) {
            statement.name = target->name;
        } else {
            statement.assignmentTarget = std::move(target);
        }

        consume(TokenKind::Equal, "expected '=' in assignment");
        statement.expression = parseExpression();

        consume(TokenKind::Semicolon, "expected ';' after assignment");

        return statement;
    }

    Stmt Parser::parseReturn() {
        const auto &returnToken = previous();

        Stmt statement;
        statement.kind = StmtKind::Return;
        statement.location = {
            .line = returnToken.line,
            .column = returnToken.column
        };

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

    Stmt Parser::parseWhile() {
        const auto &whileToken = previous();

        Stmt statement;
        statement.kind = StmtKind::While;
        statement.location = {
            .line = whileToken.line,
            .column = whileToken.column
        };

        consume(TokenKind::LParen, "expected '(' after 'while'");
        statement.condition = parseExpression();
        consume(TokenKind::RParen, "expected ')' after condition");
        statement.body = parseBlock();

        return statement;
    }

    Stmt Parser::parseBreak() {
        const auto &breakToken = previous();

        Stmt statement;
        statement.kind = StmtKind::Break;
        statement.location = {
            .line = breakToken.line,
            .column = breakToken.column
        };

        consume(TokenKind::Semicolon, "expected ';' after 'break'");
        return statement;
    }

    Stmt Parser::parseContinue() {
        const auto &continueToken = previous();

        Stmt statement;
        statement.kind = StmtKind::Continue;
        statement.location = {
            .line = continueToken.line,
            .column = continueToken.column
        };

        consume(TokenKind::Semicolon, "expected ';' after 'continue'");
        return statement;
    }
} // namespace JAOT
