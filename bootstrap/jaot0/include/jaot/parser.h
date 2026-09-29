#pragma once

#include "jaot/token.h"

#include <memory>
#include <stdint>
#include <string>
#include <vector>

namespace JAOT {

enum class ExprKind {
  Integer,
  Variable,
  Binary,
  Call,
};

struct Expr {
  ExprKind kind;

  int64_t integer = 0;

  std::string name;

  char op = 0;

  std::string callee;

  std::vector<std::unique_ptr<Expr>> arguments;

  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
};

enum class StmtKind {
  VarDecl,
  Expression,
  Return,
};

struct Stmt {
  StmtKind kind;

  std::string name;

  std::unique_ptr<Expr> expression;
};

struct Method {
  std::string name;

  std::vector<std::string> parameters;

  std::vector<Stmt> body;
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
