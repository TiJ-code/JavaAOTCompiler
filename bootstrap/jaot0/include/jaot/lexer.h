#pragma once

#include "jaot/token.h"

#include <string>
#include <vector>

namespace JAOT {

class Lexer {
public:
  explicit Lexer(std::string source);

  std::vector<Token> lex();

private:
  char peek() const;
  char advance();

  bool match(char expected);

  void skipWhitespace();

  Token identifierOrKeyword();
  Token integer();

  void error(const std::string &message) const;

  std::string source_;

  std::size_t offset_ = 0;
  std::size_t line_ = 1;
  std::size_t column_ = 1;
};

} // namespace JAOT
