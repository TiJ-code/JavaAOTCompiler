#pragma once

#include <cstddef>
#include <string>

namespace JAOT {
    enum class TokenKind {
        EndOfFile,

        Identifier,
        Character,
        Integer,
        ByteInteger,
        ShortInteger,
        LongInteger,

        KwClass,
        KwStatic,
        KwVoid,
        KwChar,
        KwByte,
        KwInt,
        KwShort,
        KwLong,
        KwBoolean,
        KwTrue,
        KwFalse,
        KwReturn,
        KwWhile,
        KwBreak,
        KwContinue,
        KwNull,
        KwNew,

        KwIf,
        KwElse,

        LBrace,
        RBrace,
        LParen,
        RParen,
        LBracket,
        RBracket,
        Semicolon,
        Comma,
        Dot,

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
