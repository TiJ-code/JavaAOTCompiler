lexer grammar JavaLexer;

// KEYWORDS
ABSTRACT     : 'abstract';
ASSERT       : 'assert';
BOOLEAN      : 'boolean';
BREAK        : 'break';
BYTE         : 'byte';
CASE         : 'case';
CATCH        : 'catch';
CHAR         : 'char';
CLASS        : 'class';
CONST        : 'const';
CONTINUE     : 'continue';
DEFAULT      : 'default';
DO           : 'do';
DOUBLE       : 'double';
ELSE         : 'else';
ENUM         : 'enum';
EXPORTS      : 'exports';
EXTENDS      : 'extends';
FINAL        : 'final';
FINALLY      : 'finally';
FLOAT        : 'float';
FOR          : 'for';
IF           : 'if';
GOTO         : 'goto';
IMPLEMENTS   : 'implements';
IMPORT       : 'import';
INSTANCEOF   : 'instanceof';
INT          : 'int';
INTERFACE    : 'interface';
LONG         : 'long';
MODULE       : 'module';
NATIVE       : 'native';
NEW          : 'new';
NULL         : 'null';
OPEN         : 'open';
OPENS        : 'opens';
PACKAGE      : 'package';
PERMITS      : 'permits';
PRIVATE      : 'private';
PROTECTED    : 'protected';
PUBLIC       : 'public';
PROVIDES     : 'provides';
RECORD       : 'record';
REQUIRES     : 'requires';
RETURN       : 'return';
SEALED       : 'sealed';
SHORT        : 'short';
STATIC       : 'static';
STRICTFP     : 'strictfp';
SUPER        : 'super';
SWITCH       : 'switch';
SYNCHRONIZED : 'synchronized';
THIS         : 'this';
THROW        : 'throw';
THROWS       : 'throws';
TO           : 'to';
TRANSIENT    : 'transient';
TRANSITIVE   : 'transitive';
TRY          : 'try';
USES         : 'uses';
VAR          : 'var';
VOID         : 'void';
VOLATILE     : 'volatile';
WHEN         : 'when';
WHILE        : 'while';
WITH         : 'with';
YIELD        : 'yield';

TRUE         : 'true';
FALSE        : 'false';

// OPERATORS

LPAREN         : '(';
RPAREN         : ')';
LBRACE         : '{';
RBRACE         : '}';
LBRACK         : '[';
RBRACK         : ']';

SEMI           : ';';
COMMA          : ',';
DOT            : '.';
ELLIPSIS       : '...';
AT             : '@';
COLON          : ':';
DOUBLECOLON    : '::';
ARROW          : '->';

ASSIGN         : '=';
GT             : '>';
LT             : '<';
BANG           : '!';
TILDE          : '~';
QUESTION       : '?';
EQUAL          : '==';
LE             : '<=';
GE             : '>=';
NOTEQUAL       : '!=';
AND            : '&&';
OR             : '||';
INC            : '++';
DEC            : '--';

ADD            : '+';
SUB            : '-';
MUL            : '*';
DIV            : '/';
MOD            : '%';
BITAND         : '&';
BITOR          : '|';
CARET          : '^';

ADD_ASSIGN     : '+=';
SUB_ASSIGN     : '-=';
MUL_ASSIGN     : '*=';
DIV_ASSIGN     : '/=';
MOD_ASSIGN     : '%=';
AND_ASSIGN     : '&=';
OR_ASSIGN      : '|=';
XOR_ASSIGN     : '^=';
LSHIFT_ASSIGN  : '<<=';
RSHIFT_ASSIGN  : '>>=';
URSHIFT_ASSIGN : '>>>=';
LSHIFT         : '<<';
RSHIFT         : '>>';
URSHIFT        : '>>>';

// LITERALS

IntegerLiteral
	: DecimalIntegerLiteral
	;

fragment DecimalIntegerLiteral
	: DecimalNumeral IntegerTypeSuffix?
	;

fragment DecimalNumeral
	: '0'
	| NonZeroDigit Digits?
	;

fragment Digits
	: Digit+
	;

fragment Digit
	: [0-9]
	;

fragment NonZeroDigit
	: [1-9]
	;

fragment IntegerTypeSuffix
	: [lL]
	;

FloatingPointLiteral
	: [0-9]+ '.' [0-9]* ExponentPart? FloatTypeSuffix?
	| '.' [0-9]+ ExponentPart? FloatTypeSuffix?
	| [0-9]+ ExponentPart FloatTypeSuffix?
	| [0-9]+ FloatTypeSuffix
	;

fragment ExponentPart
	: [eE] [+-]? [0-9]+
	;

fragment FloatTypeSuffix
	: [dDfF]
	;

CharacterLiteral
	: '\'' (~['\\\r\n] | EscapeSequence) '\''
	;

StringLiteral
	: '"' (~["\\\r\n] | EscapeSequence)* '"'
	;

TextBlock
	: '"""' ( . | '\r' | '\n' )*? '"""'
	;

fragment EscapeSequence
	: '\\' [btnfr"'\\]
	;

// IDENTIFIERS

Identifier
	: JavaLetter JavaLetterOrDigit*
	;

fragment JavaLetter
	: [a-zA-Z$_]
	;

fragment JavaLetterOrDigit
	: [a-zA-Z0-9$_]
	;

// WHITESPACE + COMMENTS
WS
	: [ \t\r\n\u000C]+ -> skip
	;

COMMENT
	: '/*' .*? '*/' -> skip
	;

LINE_COMMENT
	: '//' ~[\r\n]* -> skip
	;
