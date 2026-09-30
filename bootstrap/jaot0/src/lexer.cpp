#include "jaot/lexer.h"

#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace JAOT {

namespace {

const std::unordered_map<std::string, TokenKind> keywords{
    {"class", TokenKind::KwClass},   {"static", TokenKind::KwStatic},
    {"void", TokenKind::KwVoid},     {"int", TokenKind::KwInt},
    {"return", TokenKind::KwReturn},
};

}

const char *tokenKindName(TokenKind kind) {
  switch (kind) {
  case TokenKind::EndOfFile:
    return "end of file";

  case TokenKind::Identifier:
    return "identifier";

  case TokenKind::Integer:
    return "integer";

  case TokenKind::KwClass:
    return "class";

  case TokenKind::KwStatic:
    return "static";

  case TokenKind::KwVoid:
    return "void";

  case TokenKind::KwInt:
    return "int";

  case TokenKind::KwReturn:
    return "return";

  case TokenKind::LBrace:
    return "{";

  case TokenKind::RBrace:
    return "}";

  case TokenKind::LParen:
    return "(";

  case TokenKind::RParen:
    return ")";

  case TokenKind::Semicolon:
    return ";";

  case TokenKind::Comma:
    return ",";

  case TokenKind::Plus:
    return "+";

  case TokenKind::Minus:
    return "-";

  case TokenKind::Star:
    return "*";

  case TokenKind::Slash:
    return "/";

  case TokenKind::Equal:
    return "=";
  }

  return "unknown";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek() const {
  if (offset_ >= source_.size()) {
    return 0;
  }

  return source_[offset_];
}

char Lexer::advance() {
  const char c = peek();

  if (c == 0) {
    return c;
  }

  ++offset_;

  if (c == '\n') {
    ++line_;
    column_ = 1;
  } else {
    ++column_;
  }

  return c;
}

bool Lexer::match(char expected) {
  if (peek() != expected) {
    return false;
  }

  advance();
  return true;
}

void Lexer::skipWhitespace() {
  while (true) {
    const char c = peek();

    if (std::isspace(static_cast<unsigned char>(c))) {
      advance();
      continue;
    }

    if (c == '/' && offset_ + 1 < source_.size() &&
        source_[offset_ + 1] == '/') {
      while (peek() != '\n' && peek() != 0) {
        advance();
      }

      continue;
    }

    break;
  }
}

Token Lexer::identifierOrKeyword() {
  const std::size_t startLine = line_;
  const std::size_t startColumn = column_;

  std::string text;

  while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
    text += advance();
  }

  const auto it = keywords.find(text);

  const TokenKind kind =
      it == keywords.end() ? TokenKind::Identifier : it->second;

  return {kind, text, startLine, startColumn};
}

Token Lexer::integer() {
  const std::size_t startLine = line_;
  const std::size_t startColumn = column_;

  std::string text;

  while (std::isdigit(static_cast<unsigned char>(peek()))) {
    text += advance();
  }

  return {TokenKind::Integer, text, startLine, startColumn};
}

void Lexer::error(const std::string &message) const {
  throw std::runtime_error("lexer error at " + std::to_string(line_) + ":" +
                           std::to_string(column_) + ": " + message);
}

std::vector<Token> Lexer::lex() {
  std::vector<Token> tokens;

  while (true) {
    skipWhitespace();

    const std::size_t line = line_;
    const std::size_t column = column_;

    const char c = peek();

    if (c == 0) {
      tokens.push_back({TokenKind::EndOfFile, "", line, column});

      return tokens;
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
      tokens.push_back(identifierOrKeyword());
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
      tokens.push_back(integer());
      continue;
    }

    advance();

    switch (c) {
    case '{':
      tokens.push_back({TokenKind::LBrace, "{", line, column});
      break;

    case '}':
      tokens.push_back({TokenKind::RBrace, "}", line, column});
      break;

    case '(':
      tokens.push_back({TokenKind::LParen, "(", line, column});
      break;

    case ')':
      tokens.push_back({TokenKind::RParen, ")", line, column});
      break;

    case ';':
      tokens.push_back({TokenKind::Semicolon, ";", line, column});
      break;

    case ',':
      tokens.push_back({TokenKind::Comma, ",", line, column});
      break;

    case '+':
      tokens.push_back({TokenKind::Plus, "+", line, column});
      break;

    case '-':
      tokens.push_back({TokenKind::Minus, "-", line, column});
      break;

    case '*':
      tokens.push_back({TokenKind::Star, "*", line, column});
      break;

    case '/':
      tokens.push_back({TokenKind::Slash, "/", line, column});
      break;

    case '=':
      tokens.push_back({TokenKind::Equal, "=", line, column});
      break;

    default:
      error(std::string("unexpected character '") + c + "'");
    }
  }
}

} // namespace JAOT
