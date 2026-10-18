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
        if (check(TokenKind::KwInt) || check(TokenKind::KwBoolean)) {
            const ParsedType type = parseType(false);
            return parseVariableDeclaration(type.type, type.className);
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

        if (check(TokenKind::LBrace)) {
            Stmt statement;
            statement.kind = StmtKind::Block;
            statement.location = { current().line, current().column };
            statement.body = parseBlock();
            return statement;
        }

        if (check(TokenKind::Identifier)) {
            std::size_t lookahead = index_ + 1;

            while (lookahead + 1 < tokens_.size() &&
                   tokens_[lookahead].kind == TokenKind::Dot &&
                   tokens_[lookahead + 1].kind == TokenKind::Identifier) {
                lookahead += 2;
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
            return parseVariableDeclaration(type.type, type.className);
        }

        Stmt statement;
        statement.kind = StmtKind::Expression;
        statement.expression = parseExpression();
        statement.location = statement.expression->location;

        consume(TokenKind::Semicolon, "expected ';' after expression");

        return statement;
    }

    Stmt Parser::parseVariableDeclaration(
        const Type type,
        std::string declaredTypeName
    ) {
        const auto &name = consume(TokenKind::Identifier, "expected variable name");

        Stmt statement;
        statement.kind = StmtKind::VarDecl;
        statement.declaredType = type;
        statement.declaredTypeName = std::move(declaredTypeName);
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

        while (match(TokenKind::Dot)) {
            const Token field =
                consume(TokenKind::Identifier, "expected field name after '.'");

            auto access = std::make_unique<Expr>();
            access->kind = ExprKind::FieldAccess;
            access->name = field.text;
            access->location = { .line = field.line, .column = identifier.column };
            access->left = std::move(target);
            target = std::move(access);
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
} // namespace JAOT
