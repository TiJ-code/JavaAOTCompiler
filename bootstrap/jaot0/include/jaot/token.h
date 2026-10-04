#pragma once

#include <cstddef>
#include <string>

namespace JAOT {

enum class TokenKind {
  EndOfFile,

  Identifier,
  Integer,

  KwClass,
  KwStatic,
  KwVoid,
  KwInt,
  KwReturn,

  LBrace,
  RBrace,
  LParen,
  RParen,
  Semicolon,
  Comma,

  Plus,
  Minus,
  Star,
  Slash,
  Equal,
};

struct Token {
  TokenKind kind;
  std::string text;

  std::size_t line;
  std::size_t column;
};

const char *tokenKindName(TokenKind kind);

} // namespace JAOT
