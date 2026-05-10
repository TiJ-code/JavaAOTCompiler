
// Generated from grammar/JavaParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaParserListener.h"


/**
 * This class provides an empty implementation of JavaParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  JavaParserBaseListener : public JavaParserListener {
public:

  virtual void enterCompilationUnit(JavaParser::CompilationUnitContext * /*ctx*/) override { }
  virtual void exitCompilationUnit(JavaParser::CompilationUnitContext * /*ctx*/) override { }

  virtual void enterPackageDeclaration(JavaParser::PackageDeclarationContext * /*ctx*/) override { }
  virtual void exitPackageDeclaration(JavaParser::PackageDeclarationContext * /*ctx*/) override { }

  virtual void enterImportDeclaration(JavaParser::ImportDeclarationContext * /*ctx*/) override { }
  virtual void exitImportDeclaration(JavaParser::ImportDeclarationContext * /*ctx*/) override { }

  virtual void enterQualifiedName(JavaParser::QualifiedNameContext * /*ctx*/) override { }
  virtual void exitQualifiedName(JavaParser::QualifiedNameContext * /*ctx*/) override { }

  virtual void enterAnnotation(JavaParser::AnnotationContext * /*ctx*/) override { }
  virtual void exitAnnotation(JavaParser::AnnotationContext * /*ctx*/) override { }

  virtual void enterElementValue(JavaParser::ElementValueContext * /*ctx*/) override { }
  virtual void exitElementValue(JavaParser::ElementValueContext * /*ctx*/) override { }

  virtual void enterTypeDeclaration(JavaParser::TypeDeclarationContext * /*ctx*/) override { }
  virtual void exitTypeDeclaration(JavaParser::TypeDeclarationContext * /*ctx*/) override { }

  virtual void enterClassDeclaration(JavaParser::ClassDeclarationContext * /*ctx*/) override { }
  virtual void exitClassDeclaration(JavaParser::ClassDeclarationContext * /*ctx*/) override { }

  virtual void enterClassModifier(JavaParser::ClassModifierContext * /*ctx*/) override { }
  virtual void exitClassModifier(JavaParser::ClassModifierContext * /*ctx*/) override { }

  virtual void enterSuperclass(JavaParser::SuperclassContext * /*ctx*/) override { }
  virtual void exitSuperclass(JavaParser::SuperclassContext * /*ctx*/) override { }

  virtual void enterSuperinterfaces(JavaParser::SuperinterfacesContext * /*ctx*/) override { }
  virtual void exitSuperinterfaces(JavaParser::SuperinterfacesContext * /*ctx*/) override { }

  virtual void enterInterfaceDeclaration(JavaParser::InterfaceDeclarationContext * /*ctx*/) override { }
  virtual void exitInterfaceDeclaration(JavaParser::InterfaceDeclarationContext * /*ctx*/) override { }

  virtual void enterEnumDeclaration(JavaParser::EnumDeclarationContext * /*ctx*/) override { }
  virtual void exitEnumDeclaration(JavaParser::EnumDeclarationContext * /*ctx*/) override { }

  virtual void enterRecordDeclaration(JavaParser::RecordDeclarationContext * /*ctx*/) override { }
  virtual void exitRecordDeclaration(JavaParser::RecordDeclarationContext * /*ctx*/) override { }

  virtual void enterRecordComponentList(JavaParser::RecordComponentListContext * /*ctx*/) override { }
  virtual void exitRecordComponentList(JavaParser::RecordComponentListContext * /*ctx*/) override { }

  virtual void enterRecordComponent(JavaParser::RecordComponentContext * /*ctx*/) override { }
  virtual void exitRecordComponent(JavaParser::RecordComponentContext * /*ctx*/) override { }

  virtual void enterInterfaceBody(JavaParser::InterfaceBodyContext * /*ctx*/) override { }
  virtual void exitInterfaceBody(JavaParser::InterfaceBodyContext * /*ctx*/) override { }

  virtual void enterInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext * /*ctx*/) override { }
  virtual void exitInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext * /*ctx*/) override { }

  virtual void enterEnumBody(JavaParser::EnumBodyContext * /*ctx*/) override { }
  virtual void exitEnumBody(JavaParser::EnumBodyContext * /*ctx*/) override { }

  virtual void enterEnumConstantList(JavaParser::EnumConstantListContext * /*ctx*/) override { }
  virtual void exitEnumConstantList(JavaParser::EnumConstantListContext * /*ctx*/) override { }

  virtual void enterEnumConstant(JavaParser::EnumConstantContext * /*ctx*/) override { }
  virtual void exitEnumConstant(JavaParser::EnumConstantContext * /*ctx*/) override { }

  virtual void enterClassBody(JavaParser::ClassBodyContext * /*ctx*/) override { }
  virtual void exitClassBody(JavaParser::ClassBodyContext * /*ctx*/) override { }

  virtual void enterClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext * /*ctx*/) override { }
  virtual void exitClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext * /*ctx*/) override { }

  virtual void enterMemberDeclaration(JavaParser::MemberDeclarationContext * /*ctx*/) override { }
  virtual void exitMemberDeclaration(JavaParser::MemberDeclarationContext * /*ctx*/) override { }

  virtual void enterModifier(JavaParser::ModifierContext * /*ctx*/) override { }
  virtual void exitModifier(JavaParser::ModifierContext * /*ctx*/) override { }

  virtual void enterFieldDeclaration(JavaParser::FieldDeclarationContext * /*ctx*/) override { }
  virtual void exitFieldDeclaration(JavaParser::FieldDeclarationContext * /*ctx*/) override { }

  virtual void enterVariableDeclarators(JavaParser::VariableDeclaratorsContext * /*ctx*/) override { }
  virtual void exitVariableDeclarators(JavaParser::VariableDeclaratorsContext * /*ctx*/) override { }

  virtual void enterVariableDeclarator(JavaParser::VariableDeclaratorContext * /*ctx*/) override { }
  virtual void exitVariableDeclarator(JavaParser::VariableDeclaratorContext * /*ctx*/) override { }

  virtual void enterVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext * /*ctx*/) override { }
  virtual void exitVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext * /*ctx*/) override { }

  virtual void enterVariableInitializer(JavaParser::VariableInitializerContext * /*ctx*/) override { }
  virtual void exitVariableInitializer(JavaParser::VariableInitializerContext * /*ctx*/) override { }

  virtual void enterVariableModifier(JavaParser::VariableModifierContext * /*ctx*/) override { }
  virtual void exitVariableModifier(JavaParser::VariableModifierContext * /*ctx*/) override { }

  virtual void enterArrayInitializer(JavaParser::ArrayInitializerContext * /*ctx*/) override { }
  virtual void exitArrayInitializer(JavaParser::ArrayInitializerContext * /*ctx*/) override { }

  virtual void enterMethodDeclaration(JavaParser::MethodDeclarationContext * /*ctx*/) override { }
  virtual void exitMethodDeclaration(JavaParser::MethodDeclarationContext * /*ctx*/) override { }

  virtual void enterConstructorDeclaration(JavaParser::ConstructorDeclarationContext * /*ctx*/) override { }
  virtual void exitConstructorDeclaration(JavaParser::ConstructorDeclarationContext * /*ctx*/) override { }

  virtual void enterConstructorModifier(JavaParser::ConstructorModifierContext * /*ctx*/) override { }
  virtual void exitConstructorModifier(JavaParser::ConstructorModifierContext * /*ctx*/) override { }

  virtual void enterThrows_(JavaParser::Throws_Context * /*ctx*/) override { }
  virtual void exitThrows_(JavaParser::Throws_Context * /*ctx*/) override { }

  virtual void enterQualifiedNameList(JavaParser::QualifiedNameListContext * /*ctx*/) override { }
  virtual void exitQualifiedNameList(JavaParser::QualifiedNameListContext * /*ctx*/) override { }

  virtual void enterMethodBody(JavaParser::MethodBodyContext * /*ctx*/) override { }
  virtual void exitMethodBody(JavaParser::MethodBodyContext * /*ctx*/) override { }

  virtual void enterFormalParameters(JavaParser::FormalParametersContext * /*ctx*/) override { }
  virtual void exitFormalParameters(JavaParser::FormalParametersContext * /*ctx*/) override { }

  virtual void enterFormalParameterList(JavaParser::FormalParameterListContext * /*ctx*/) override { }
  virtual void exitFormalParameterList(JavaParser::FormalParameterListContext * /*ctx*/) override { }

  virtual void enterFormalParameter(JavaParser::FormalParameterContext * /*ctx*/) override { }
  virtual void exitFormalParameter(JavaParser::FormalParameterContext * /*ctx*/) override { }

  virtual void enterTypeType(JavaParser::TypeTypeContext * /*ctx*/) override { }
  virtual void exitTypeType(JavaParser::TypeTypeContext * /*ctx*/) override { }

  virtual void enterPrimitiveType(JavaParser::PrimitiveTypeContext * /*ctx*/) override { }
  virtual void exitPrimitiveType(JavaParser::PrimitiveTypeContext * /*ctx*/) override { }

  virtual void enterClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext * /*ctx*/) override { }
  virtual void exitClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext * /*ctx*/) override { }

  virtual void enterTypeArguments(JavaParser::TypeArgumentsContext * /*ctx*/) override { }
  virtual void exitTypeArguments(JavaParser::TypeArgumentsContext * /*ctx*/) override { }

  virtual void enterTypeArgument(JavaParser::TypeArgumentContext * /*ctx*/) override { }
  virtual void exitTypeArgument(JavaParser::TypeArgumentContext * /*ctx*/) override { }

  virtual void enterTypeParameters(JavaParser::TypeParametersContext * /*ctx*/) override { }
  virtual void exitTypeParameters(JavaParser::TypeParametersContext * /*ctx*/) override { }

  virtual void enterTypeParameter(JavaParser::TypeParameterContext * /*ctx*/) override { }
  virtual void exitTypeParameter(JavaParser::TypeParameterContext * /*ctx*/) override { }

  virtual void enterTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext * /*ctx*/) override { }
  virtual void exitTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext * /*ctx*/) override { }

  virtual void enterTypeList(JavaParser::TypeListContext * /*ctx*/) override { }
  virtual void exitTypeList(JavaParser::TypeListContext * /*ctx*/) override { }

  virtual void enterDims(JavaParser::DimsContext * /*ctx*/) override { }
  virtual void exitDims(JavaParser::DimsContext * /*ctx*/) override { }

  virtual void enterBlock(JavaParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(JavaParser::BlockContext * /*ctx*/) override { }

  virtual void enterBlockStatement(JavaParser::BlockStatementContext * /*ctx*/) override { }
  virtual void exitBlockStatement(JavaParser::BlockStatementContext * /*ctx*/) override { }

  virtual void enterLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext * /*ctx*/) override { }
  virtual void exitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext * /*ctx*/) override { }

  virtual void enterLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext * /*ctx*/) override { }
  virtual void exitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext * /*ctx*/) override { }

  virtual void enterStatement(JavaParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(JavaParser::StatementContext * /*ctx*/) override { }

  virtual void enterSwitchBlock(JavaParser::SwitchBlockContext * /*ctx*/) override { }
  virtual void exitSwitchBlock(JavaParser::SwitchBlockContext * /*ctx*/) override { }

  virtual void enterSwitchLabel(JavaParser::SwitchLabelContext * /*ctx*/) override { }
  virtual void exitSwitchLabel(JavaParser::SwitchLabelContext * /*ctx*/) override { }

  virtual void enterCatchClause(JavaParser::CatchClauseContext * /*ctx*/) override { }
  virtual void exitCatchClause(JavaParser::CatchClauseContext * /*ctx*/) override { }

  virtual void enterCatchType(JavaParser::CatchTypeContext * /*ctx*/) override { }
  virtual void exitCatchType(JavaParser::CatchTypeContext * /*ctx*/) override { }

  virtual void enterFinallyBlock(JavaParser::FinallyBlockContext * /*ctx*/) override { }
  virtual void exitFinallyBlock(JavaParser::FinallyBlockContext * /*ctx*/) override { }

  virtual void enterForControl(JavaParser::ForControlContext * /*ctx*/) override { }
  virtual void exitForControl(JavaParser::ForControlContext * /*ctx*/) override { }

  virtual void enterEnhancedForControl(JavaParser::EnhancedForControlContext * /*ctx*/) override { }
  virtual void exitEnhancedForControl(JavaParser::EnhancedForControlContext * /*ctx*/) override { }

  virtual void enterForInit(JavaParser::ForInitContext * /*ctx*/) override { }
  virtual void exitForInit(JavaParser::ForInitContext * /*ctx*/) override { }

  virtual void enterForUpdate(JavaParser::ForUpdateContext * /*ctx*/) override { }
  virtual void exitForUpdate(JavaParser::ForUpdateContext * /*ctx*/) override { }

  virtual void enterExpressionList(JavaParser::ExpressionListContext * /*ctx*/) override { }
  virtual void exitExpressionList(JavaParser::ExpressionListContext * /*ctx*/) override { }

  virtual void enterStatementExpression(JavaParser::StatementExpressionContext * /*ctx*/) override { }
  virtual void exitStatementExpression(JavaParser::StatementExpressionContext * /*ctx*/) override { }

  virtual void enterParExpression(JavaParser::ParExpressionContext * /*ctx*/) override { }
  virtual void exitParExpression(JavaParser::ParExpressionContext * /*ctx*/) override { }

  virtual void enterExpression(JavaParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(JavaParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterLambdaExpression(JavaParser::LambdaExpressionContext * /*ctx*/) override { }
  virtual void exitLambdaExpression(JavaParser::LambdaExpressionContext * /*ctx*/) override { }

  virtual void enterLambdaParameters(JavaParser::LambdaParametersContext * /*ctx*/) override { }
  virtual void exitLambdaParameters(JavaParser::LambdaParametersContext * /*ctx*/) override { }

  virtual void enterLambdaBody(JavaParser::LambdaBodyContext * /*ctx*/) override { }
  virtual void exitLambdaBody(JavaParser::LambdaBodyContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(JavaParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(JavaParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterLeftHandSide(JavaParser::LeftHandSideContext * /*ctx*/) override { }
  virtual void exitLeftHandSide(JavaParser::LeftHandSideContext * /*ctx*/) override { }

  virtual void enterAssignmentOperator(JavaParser::AssignmentOperatorContext * /*ctx*/) override { }
  virtual void exitAssignmentOperator(JavaParser::AssignmentOperatorContext * /*ctx*/) override { }

  virtual void enterConditionalExpression(JavaParser::ConditionalExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalExpression(JavaParser::ConditionalExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalOrExpression(JavaParser::ConditionalOrExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalOrExpression(JavaParser::ConditionalOrExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalAndExpression(JavaParser::ConditionalAndExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalAndExpression(JavaParser::ConditionalAndExpressionContext * /*ctx*/) override { }

  virtual void enterInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext * /*ctx*/) override { }
  virtual void exitInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext * /*ctx*/) override { }

  virtual void enterExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext * /*ctx*/) override { }
  virtual void exitExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext * /*ctx*/) override { }

  virtual void enterAndExpression(JavaParser::AndExpressionContext * /*ctx*/) override { }
  virtual void exitAndExpression(JavaParser::AndExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(JavaParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(JavaParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(JavaParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(JavaParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterPattern(JavaParser::PatternContext * /*ctx*/) override { }
  virtual void exitPattern(JavaParser::PatternContext * /*ctx*/) override { }

  virtual void enterShiftExpression(JavaParser::ShiftExpressionContext * /*ctx*/) override { }
  virtual void exitShiftExpression(JavaParser::ShiftExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(JavaParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(JavaParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpression(JavaParser::UnaryExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryExpression(JavaParser::UnaryExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext * /*ctx*/) override { }
  virtual void exitUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext * /*ctx*/) override { }

  virtual void enterCastExpression(JavaParser::CastExpressionContext * /*ctx*/) override { }
  virtual void exitCastExpression(JavaParser::CastExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixExpression(JavaParser::PostfixExpressionContext * /*ctx*/) override { }
  virtual void exitPostfixExpression(JavaParser::PostfixExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixPart(JavaParser::PostfixPartContext * /*ctx*/) override { }
  virtual void exitPostfixPart(JavaParser::PostfixPartContext * /*ctx*/) override { }

  virtual void enterPrimary(JavaParser::PrimaryContext * /*ctx*/) override { }
  virtual void exitPrimary(JavaParser::PrimaryContext * /*ctx*/) override { }

  virtual void enterCreator(JavaParser::CreatorContext * /*ctx*/) override { }
  virtual void exitCreator(JavaParser::CreatorContext * /*ctx*/) override { }

  virtual void enterCreatedName(JavaParser::CreatedNameContext * /*ctx*/) override { }
  virtual void exitCreatedName(JavaParser::CreatedNameContext * /*ctx*/) override { }

  virtual void enterClassCreatorRest(JavaParser::ClassCreatorRestContext * /*ctx*/) override { }
  virtual void exitClassCreatorRest(JavaParser::ClassCreatorRestContext * /*ctx*/) override { }

  virtual void enterArrayCreatorRest(JavaParser::ArrayCreatorRestContext * /*ctx*/) override { }
  virtual void exitArrayCreatorRest(JavaParser::ArrayCreatorRestContext * /*ctx*/) override { }

  virtual void enterArguments(JavaParser::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(JavaParser::ArgumentsContext * /*ctx*/) override { }

  virtual void enterMethodInvocation(JavaParser::MethodInvocationContext * /*ctx*/) override { }
  virtual void exitMethodInvocation(JavaParser::MethodInvocationContext * /*ctx*/) override { }

  virtual void enterFieldAccess(JavaParser::FieldAccessContext * /*ctx*/) override { }
  virtual void exitFieldAccess(JavaParser::FieldAccessContext * /*ctx*/) override { }

  virtual void enterArrayAccess(JavaParser::ArrayAccessContext * /*ctx*/) override { }
  virtual void exitArrayAccess(JavaParser::ArrayAccessContext * /*ctx*/) override { }

  virtual void enterExpressionName(JavaParser::ExpressionNameContext * /*ctx*/) override { }
  virtual void exitExpressionName(JavaParser::ExpressionNameContext * /*ctx*/) override { }

  virtual void enterAmbiguousName(JavaParser::AmbiguousNameContext * /*ctx*/) override { }
  virtual void exitAmbiguousName(JavaParser::AmbiguousNameContext * /*ctx*/) override { }

  virtual void enterLiteral(JavaParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(JavaParser::LiteralContext * /*ctx*/) override { }

  virtual void enterTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext * /*ctx*/) override { }
  virtual void exitTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

