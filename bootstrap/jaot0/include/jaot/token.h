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
        KwBoolean,
        KwTrue,
        KwFalse,
        KwReturn,
        KwWhile,

        KwIf,
        KwElse,

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

        EqualEqual,
        BangEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,

        Bang,
        And,
        Or,
    };

    struct Token {
        TokenKind kind;
        std::string text;

        std::size_t line;
        std::size_t column;
    };

    const char *tokenKindName(TokenKind kind);
} // namespace JAOT
