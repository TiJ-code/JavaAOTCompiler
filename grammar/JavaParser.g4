parser grammar JavaParser;

options {
	tokenVocab = JavaLexer;
}

compilationUnit
	: packageDeclaration?
	  importDeclaration*
	  (typeDeclaration | memberDeclaration)*
	  EOF
	;

packageDeclaration
	: annotation* PACKAGE qualifiedName SEMI
	;

importDeclaration
	: IMPORT (STATIC | MODULE)? qualifiedName (DOT MUL)* SEMI
	;

qualifiedName
	: Identifier (DOT Identifier)*
	;

annotation
	: AT qualifiedName
	  (LPAREN elementValue? RPAREN)?
	;

elementValue
	: expression
	;

// TYPES

typeDeclaration
	: classDeclaration
	| interfaceDeclaration
	| enumDeclaration
	| recordDeclaration
	| SEMI
	;

classDeclaration
	: classModifier*
	  CLASS Identifier
	  typeParameters?
	  superclass?
	  superinterfaces?
	  classBody
	;

classModifier
	: annotation
	| PUBLIC
	| PROTECTED
	| PRIVATE
	| ABSTRACT
	| STATIC
	| FINAL
	| SEALED
	;

superclass
	: EXTENDS typeType
	;

superinterfaces
	: IMPLEMENTS typeList
	;

interfaceDeclaration
	: INTERFACE Identifier interfaceBody
	;

enumDeclaration
	: ENUM Identifier enumBody
	;

recordDeclaration
	: RECORD Identifier
	  LPAREN recordComponentList? RPAREN
	  classBody
	;

recordComponentList
	: recordComponent (COMMA recordComponent)*
	;

recordComponent
	: typeType Identifier
	;

interfaceBody
	: LBRACE interfaceBodyDeclaration* RBRACE
	;

interfaceBodyDeclaration
	: memberDeclaration
	| SEMI
	;

enumBody
	: LBRACE enumConstantList? RBRACE
	;

enumConstantList
	: enumConstant (COMMA enumConstant)*
	;

enumConstant
	: annotation* Identifier
	;

classBody
	: LBRACE classBodyDeclaration* RBRACE
	;

classBodyDeclaration
	: memberDeclaration
	| block
	| SEMI
	;

memberDeclaration
	: modifier* methodDeclaration
	| modifier* fieldDeclaration
	| modifier* constructorDeclaration
	| classDeclaration
	;

modifier
	: annotation
	| PUBLIC
	| PRIVATE
	| PROTECTED
	| STATIC
	| FINAL
	| ABSTRACT
	| NATIVE
	| SYNCHRONIZED
	| VOLATILE
	| TRANSIENT
	| STRICTFP
	;

fieldDeclaration
	: typeType variableDeclarators SEMI
	;

variableDeclarators
	: variableDeclarator (COMMA variableDeclarator)*
	;

variableDeclarator
	: variableDeclaratorId (ASSIGN variableInitializer)?
	;

variableDeclaratorId
	: (Identifier | UNDERSCORE) dims?
	;

variableInitializer
	: expression
	| arrayInitializer
	;

variableModifier
    : annotation
    | FINAL
    ;

arrayInitializer
	: LBRACE
	  (variableInitializer
	   (COMMA variableInitializer)*)?
	  COMMA?
	  RBRACE
	;

methodDeclaration
	: typeTypeOrVoid
	  Identifier
	  formalParameters
	  dims?
	  throws_?
	  methodBody
	;

constructorDeclaration
	: constructorModifier* Identifier formalParameters throws_? methodBody
	;

constructorModifier
    : annotation
    | PUBLIC
    | PROTECTED
    | PRIVATE
    ;

throws_
	: THROWS qualifiedNameList
	;

qualifiedNameList
	: qualifiedName (COMMA qualifiedName)*
	;

methodBody
	: block
	| SEMI
	;

formalParameters
	: LPAREN formalParameterList? RPAREN
	;

formalParameterList
	: formalParameter (COMMA formalParameter)*
	;

formalParameter
	: typeType variableDeclaratorId
	;

// TYPES

typeType
	: primitiveType dims?
	| classOrInterfaceType dims?
	;

primitiveType
	: BOOLEAN
	| CHAR
	| BYTE
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	;

classOrInterfaceType
	: Identifier typeArguments?
 	  (DOT Identifier typeArguments?)*
	;

typeArguments
	: LT typeArgument (COMMA typeArgument)* GT
	;

typeArgument
	: typeType
	| QUESTION
	| QUESTION EXTENDS typeType
	| QUESTION SUPER typeType
	;

typeParameters
	: LT typeParameter (COMMA typeParameter)* GT
	;

typeParameter
	: Identifier (EXTENDS typeType)?
	;

typeTypeOrVoid
	: typeType
	| VOID
	;

typeList
	: typeType (COMMA typeType)*
	;

dims
	: (LBRACK RBRACK)+
	;

// STATEMENTS

block
	: LBRACE blockStatement* RBRACE
	;

blockStatement
	: localVariableDeclarationStatement
	| statement
	;

localVariableDeclarationStatement
	: localVariableDeclaration SEMI
	;

localVariableDeclaration
	: variableModifier* (typeType | VAR) variableDeclarators
	;

statement
	: block
	| ASSERT expression (COLON expression)? SEMI
	| IF parExpression statement (ELSE statement)?
	| FOR LPAREN forControl RPAREN statement
	| WHILE parExpression statement
	| DO statement WHILE parExpression SEMI
	| TRY block catchClause* finallyBlock?
	| SWITCH parExpression switchBlock
	| RETURN expression? SEMI
	| THROW expression SEMI
	| BREAK Identifier? SEMI
	| CONTINUE Identifier? SEMI
	| SEMI
	| statementExpression SEMI
	;

switchBlock
	: LBRACE switchLabel* RBRACE
	;

switchLabel
	: CASE (expression | pattern) (ARROW (statement | block) | COLON)
	| DEFAULT (ARROW (statement | block) | COLON)
	;

catchClause
	: CATCH LPAREN catchType Identifier RPAREN block
	;

catchType
	: qualifiedName
	;

finallyBlock
	: FINALLY block
	;

forControl
	: enhancedForControl
	| forInit? SEMI expression? SEMI forUpdate?
	;

enhancedForControl
	: typeType Identifier COLON expression
	;

forInit
	: localVariableDeclaration
	| expressionList
	;

forUpdate
	: expressionList
	;

expressionList
	: expression (COMMA expression)*
	;

statementExpression
	: expression
	;

parExpression
	: LPAREN expression RPAREN
	;

// EXPRESSIONS

expression
	: lambdaExpression
	| assignmentExpression
	;

lambdaExpression
	: lambdaParameters ARROW lambdaBody
	;

lambdaParameters
	: Identifier
	| LPAREN formalParameterList? RPAREN
	;

lambdaBody
	: expression
	| block
	;

assignmentExpression
	: conditionalExpression
	| leftHandSide assignmentOperator expression
	;

leftHandSide
	: expressionName
	| fieldAccess
	| arrayAccess
	;

assignmentOperator
	: ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| AND_ASSIGN
	| OR_ASSIGN
	| XOR_ASSIGN
	| LSHIFT_ASSIGN
	| RSHIFT_ASSIGN
	| URSHIFT_ASSIGN
	;

conditionalExpression
	: conditionalOrExpression
	  (QUESTION expression COLON conditionalExpression)?
	;

conditionalOrExpression
	: conditionalAndExpression
	  (OR conditionalAndExpression)*
	;

conditionalAndExpression
	: inclusiveOrExpression
	  (AND inclusiveOrExpression)*
	;

inclusiveOrExpression
	: exclusiveOrExpression
	  (BITOR exclusiveOrExpression)*
	;

exclusiveOrExpression
	: andExpression
	  (CARET andExpression)*
	;

andExpression
	: equalityExpression
	  (BITAND equalityExpression)*
	;

equalityExpression
	: relationalExpression
	  ((EQUAL | NOTEQUAL) relationalExpression)*
	;

relationalExpression
	: shiftExpression
	  (
		(LT | GT | LE | GE) shiftExpression
		| INSTANCEOF (typeType | pattern)
	  )*
	;

pattern
    : typeType variableDeclaratorId
    ;

shiftExpression
	: additiveExpression
	  ((LSHIFT | RSHIFT | URSHIFT) additiveExpression)*
	;

additiveExpression
	: multiplicativeExpression
	  ((ADD | SUB) multiplicativeExpression)*
	;

multiplicativeExpression
	: unaryExpression
	  ((MUL | DIV | MOD) unaryExpression)*
	;

unaryExpression
	: INC unaryExpression
	| DEC unaryExpression
	| ADD unaryExpression
	| SUB unaryExpression
	| unaryExpressionNotPlusMinus
	;

unaryExpressionNotPlusMinus
	: postfixExpression
	| TILDE unaryExpression
	| BANG unaryExpression
	| castExpression
	;

castExpression
	: LPAREN primitiveType RPAREN unaryExpression
	| LPAREN typeType RPAREN unaryExpressionNotPlusMinus
	;

postfixExpression
	: primary postfixPart* (INC | DEC)?
	;

postfixPart
	: DOT Identifier
	| DOT Identifier arguments
	| LBRACK expression RBRACK
	;

primary
	: literal
	| THIS
	| SUPER
	| LPAREN expression RPAREN
	| creator
	| expressionName
	;

creator
	: NEW createdName classCreatorRest
	| NEW createdName arrayCreatorRest
	| NEW primitiveType arrayCreatorRest
	;

createdName
	: Identifier typeArgumentsOrDiamond?
	  (DOT Identifier typeArgumentsOrDiamond?)*
	;

classCreatorRest
	: arguments classBody?
	;

arrayCreatorRest
	: LBRACK expression RBRACK (LBRACK expression RBRACK)* dims?
	| dims arrayInitializer
	;

arguments
	: LPAREN expressionList? RPAREN
	;

methodInvocation
	: expressionName arguments
	| primary DOT Identifier arguments
	;

fieldAccess
	: primary DOT Identifier
	;

arrayAccess
	: expressionName LBRACK expression RBRACK
	;

expressionName
	: Identifier
	| ambiguousName DOT Identifier
	;

ambiguousName
	: Identifier
	| ambiguousName DOT Identifier
	;

literal
	: IntegerLiteral
	| FloatingPointLiteral
	| CharacterLiteral
	| StringLiteral
	| TextBlock
	| TRUE
	| FALSE
	| NULL
	;

typeArgumentsOrDiamond
	: LT GT
	| typeArguments
	;
