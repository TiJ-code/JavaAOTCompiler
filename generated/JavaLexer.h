
// Generated from grammar/JavaLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaLexer : public antlr4::Lexer {
public:
  enum {
    ABSTRACT = 1, ASSERT = 2, BOOLEAN = 3, BREAK = 4, BYTE = 5, CASE = 6, 
    CATCH = 7, CHAR = 8, CLASS = 9, CONST = 10, CONTINUE = 11, DEFAULT = 12, 
    DO = 13, DOUBLE = 14, ELSE = 15, ENUM = 16, EXPORTS = 17, EXTENDS = 18, 
    FINAL = 19, FINALLY = 20, FLOAT = 21, FOR = 22, IF = 23, GOTO = 24, 
    IMPLEMENTS = 25, IMPORT = 26, INSTANCEOF = 27, INT = 28, INTERFACE = 29, 
    LONG = 30, MODULE = 31, NATIVE = 32, NEW = 33, NULL_ = 34, OPEN = 35, 
    OPENS = 36, PACKAGE = 37, PERMITS = 38, PRIVATE = 39, PROTECTED = 40, 
    PUBLIC = 41, PROVIDES = 42, RECORD = 43, REQUIRES = 44, RETURN = 45, 
    SEALED = 46, SHORT = 47, STATIC = 48, STRICTFP = 49, SUPER = 50, SWITCH = 51, 
    SYNCHRONIZED = 52, THIS = 53, THROW = 54, THROWS = 55, TO = 56, TRANSIENT = 57, 
    TRANSITIVE = 58, TRY = 59, USES = 60, VAR = 61, VOID = 62, VOLATILE = 63, 
    WHEN = 64, WHILE = 65, WITH = 66, YIELD = 67, TRUE = 68, FALSE = 69, 
    LPAREN = 70, RPAREN = 71, LBRACE = 72, RBRACE = 73, LBRACK = 74, RBRACK = 75, 
    SEMI = 76, COMMA = 77, DOT = 78, ELLIPSIS = 79, AT = 80, COLON = 81, 
    DOUBLECOLON = 82, ARROW = 83, ASSIGN = 84, GT = 85, LT = 86, BANG = 87, 
    TILDE = 88, QUESTION = 89, EQUAL = 90, LE = 91, GE = 92, NOTEQUAL = 93, 
    AND = 94, OR = 95, INC = 96, DEC = 97, ADD = 98, SUB = 99, MUL = 100, 
    DIV = 101, MOD = 102, BITAND = 103, BITOR = 104, CARET = 105, ADD_ASSIGN = 106, 
    SUB_ASSIGN = 107, MUL_ASSIGN = 108, DIV_ASSIGN = 109, MOD_ASSIGN = 110, 
    AND_ASSIGN = 111, OR_ASSIGN = 112, XOR_ASSIGN = 113, LSHIFT_ASSIGN = 114, 
    RSHIFT_ASSIGN = 115, URSHIFT_ASSIGN = 116, LSHIFT = 117, RSHIFT = 118, 
    URSHIFT = 119, UNDERSCORE = 120, IntegerLiteral = 121, FloatingPointLiteral = 122, 
    CharacterLiteral = 123, StringLiteral = 124, TextBlock = 125, Identifier = 126, 
    WS = 127, COMMENT = 128, LINE_COMMENT = 129
  };

  explicit JavaLexer(antlr4::CharStream *input);

  ~JavaLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

