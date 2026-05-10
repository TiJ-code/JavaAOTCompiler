
// Generated from grammar/JavaParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by JavaParser.
 */
class  JavaParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterCompilationUnit(JavaParser::CompilationUnitContext *ctx) = 0;
  virtual void exitCompilationUnit(JavaParser::CompilationUnitContext *ctx) = 0;

  virtual void enterPackageDeclaration(JavaParser::PackageDeclarationContext *ctx) = 0;
  virtual void exitPackageDeclaration(JavaParser::PackageDeclarationContext *ctx) = 0;

  virtual void enterImportDeclaration(JavaParser::ImportDeclarationContext *ctx) = 0;
  virtual void exitImportDeclaration(JavaParser::ImportDeclarationContext *ctx) = 0;

  virtual void enterQualifiedName(JavaParser::QualifiedNameContext *ctx) = 0;
  virtual void exitQualifiedName(JavaParser::QualifiedNameContext *ctx) = 0;

  virtual void enterAnnotation(JavaParser::AnnotationContext *ctx) = 0;
  virtual void exitAnnotation(JavaParser::AnnotationContext *ctx) = 0;

  virtual void enterElementValue(JavaParser::ElementValueContext *ctx) = 0;
  virtual void exitElementValue(JavaParser::ElementValueContext *ctx) = 0;

  virtual void enterTypeDeclaration(JavaParser::TypeDeclarationContext *ctx) = 0;
  virtual void exitTypeDeclaration(JavaParser::TypeDeclarationContext *ctx) = 0;

  virtual void enterClassDeclaration(JavaParser::ClassDeclarationContext *ctx) = 0;
  virtual void exitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) = 0;

  virtual void enterClassModifier(JavaParser::ClassModifierContext *ctx) = 0;
  virtual void exitClassModifier(JavaParser::ClassModifierContext *ctx) = 0;

  virtual void enterSuperclass(JavaParser::SuperclassContext *ctx) = 0;
  virtual void exitSuperclass(JavaParser::SuperclassContext *ctx) = 0;

  virtual void enterSuperinterfaces(JavaParser::SuperinterfacesContext *ctx) = 0;
  virtual void exitSuperinterfaces(JavaParser::SuperinterfacesContext *ctx) = 0;

  virtual void enterInterfaceDeclaration(JavaParser::InterfaceDeclarationContext *ctx) = 0;
  virtual void exitInterfaceDeclaration(JavaParser::InterfaceDeclarationContext *ctx) = 0;

  virtual void enterEnumDeclaration(JavaParser::EnumDeclarationContext *ctx) = 0;
  virtual void exitEnumDeclaration(JavaParser::EnumDeclarationContext *ctx) = 0;

  virtual void enterRecordDeclaration(JavaParser::RecordDeclarationContext *ctx) = 0;
  virtual void exitRecordDeclaration(JavaParser::RecordDeclarationContext *ctx) = 0;

  virtual void enterRecordComponentList(JavaParser::RecordComponentListContext *ctx) = 0;
  virtual void exitRecordComponentList(JavaParser::RecordComponentListContext *ctx) = 0;

  virtual void enterRecordComponent(JavaParser::RecordComponentContext *ctx) = 0;
  virtual void exitRecordComponent(JavaParser::RecordComponentContext *ctx) = 0;

  virtual void enterInterfaceBody(JavaParser::InterfaceBodyContext *ctx) = 0;
  virtual void exitInterfaceBody(JavaParser::InterfaceBodyContext *ctx) = 0;

  virtual void enterInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext *ctx) = 0;
  virtual void exitInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext *ctx) = 0;

  virtual void enterEnumBody(JavaParser::EnumBodyContext *ctx) = 0;
  virtual void exitEnumBody(JavaParser::EnumBodyContext *ctx) = 0;

  virtual void enterEnumConstantList(JavaParser::EnumConstantListContext *ctx) = 0;
  virtual void exitEnumConstantList(JavaParser::EnumConstantListContext *ctx) = 0;

  virtual void enterEnumConstant(JavaParser::EnumConstantContext *ctx) = 0;
  virtual void exitEnumConstant(JavaParser::EnumConstantContext *ctx) = 0;

  virtual void enterClassBody(JavaParser::ClassBodyContext *ctx) = 0;
  virtual void exitClassBody(JavaParser::ClassBodyContext *ctx) = 0;

  virtual void enterClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext *ctx) = 0;
  virtual void exitClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext *ctx) = 0;

  virtual void enterMemberDeclaration(JavaParser::MemberDeclarationContext *ctx) = 0;
  virtual void exitMemberDeclaration(JavaParser::MemberDeclarationContext *ctx) = 0;

  virtual void enterModifier(JavaParser::ModifierContext *ctx) = 0;
  virtual void exitModifier(JavaParser::ModifierContext *ctx) = 0;

  virtual void enterFieldDeclaration(JavaParser::FieldDeclarationContext *ctx) = 0;
  virtual void exitFieldDeclaration(JavaParser::FieldDeclarationContext *ctx) = 0;

  virtual void enterVariableDeclarators(JavaParser::VariableDeclaratorsContext *ctx) = 0;
  virtual void exitVariableDeclarators(JavaParser::VariableDeclaratorsContext *ctx) = 0;

  virtual void enterVariableDeclarator(JavaParser::VariableDeclaratorContext *ctx) = 0;
  virtual void exitVariableDeclarator(JavaParser::VariableDeclaratorContext *ctx) = 0;

  virtual void enterVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext *ctx) = 0;
  virtual void exitVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext *ctx) = 0;

  virtual void enterVariableInitializer(JavaParser::VariableInitializerContext *ctx) = 0;
  virtual void exitVariableInitializer(JavaParser::VariableInitializerContext *ctx) = 0;

  virtual void enterArrayInitializer(JavaParser::ArrayInitializerContext *ctx) = 0;
  virtual void exitArrayInitializer(JavaParser::ArrayInitializerContext *ctx) = 0;

  virtual void enterMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) = 0;
  virtual void exitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) = 0;

  virtual void enterConstructorDeclaration(JavaParser::ConstructorDeclarationContext *ctx) = 0;
  virtual void exitConstructorDeclaration(JavaParser::ConstructorDeclarationContext *ctx) = 0;

  virtual void enterThrows_(JavaParser::Throws_Context *ctx) = 0;
  virtual void exitThrows_(JavaParser::Throws_Context *ctx) = 0;

  virtual void enterQualifiedNameList(JavaParser::QualifiedNameListContext *ctx) = 0;
  virtual void exitQualifiedNameList(JavaParser::QualifiedNameListContext *ctx) = 0;

  virtual void enterMethodBody(JavaParser::MethodBodyContext *ctx) = 0;
  virtual void exitMethodBody(JavaParser::MethodBodyContext *ctx) = 0;

  virtual void enterFormalParameters(JavaParser::FormalParametersContext *ctx) = 0;
  virtual void exitFormalParameters(JavaParser::FormalParametersContext *ctx) = 0;

  virtual void enterFormalParameterList(JavaParser::FormalParameterListContext *ctx) = 0;
  virtual void exitFormalParameterList(JavaParser::FormalParameterListContext *ctx) = 0;

  virtual void enterFormalParameter(JavaParser::FormalParameterContext *ctx) = 0;
  virtual void exitFormalParameter(JavaParser::FormalParameterContext *ctx) = 0;

  virtual void enterTypeType(JavaParser::TypeTypeContext *ctx) = 0;
  virtual void exitTypeType(JavaParser::TypeTypeContext *ctx) = 0;

  virtual void enterPrimitiveType(JavaParser::PrimitiveTypeContext *ctx) = 0;
  virtual void exitPrimitiveType(JavaParser::PrimitiveTypeContext *ctx) = 0;

  virtual void enterClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext *ctx) = 0;
  virtual void exitClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext *ctx) = 0;

  virtual void enterTypeArguments(JavaParser::TypeArgumentsContext *ctx) = 0;
  virtual void exitTypeArguments(JavaParser::TypeArgumentsContext *ctx) = 0;

  virtual void enterTypeArgument(JavaParser::TypeArgumentContext *ctx) = 0;
  virtual void exitTypeArgument(JavaParser::TypeArgumentContext *ctx) = 0;

  virtual void enterTypeParameters(JavaParser::TypeParametersContext *ctx) = 0;
  virtual void exitTypeParameters(JavaParser::TypeParametersContext *ctx) = 0;

  virtual void enterTypeParameter(JavaParser::TypeParameterContext *ctx) = 0;
  virtual void exitTypeParameter(JavaParser::TypeParameterContext *ctx) = 0;

  virtual void enterTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext *ctx) = 0;
  virtual void exitTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext *ctx) = 0;

  virtual void enterTypeList(JavaParser::TypeListContext *ctx) = 0;
  virtual void exitTypeList(JavaParser::TypeListContext *ctx) = 0;

  virtual void enterDims(JavaParser::DimsContext *ctx) = 0;
  virtual void exitDims(JavaParser::DimsContext *ctx) = 0;

  virtual void enterBlock(JavaParser::BlockContext *ctx) = 0;
  virtual void exitBlock(JavaParser::BlockContext *ctx) = 0;

  virtual void enterBlockStatement(JavaParser::BlockStatementContext *ctx) = 0;
  virtual void exitBlockStatement(JavaParser::BlockStatementContext *ctx) = 0;

  virtual void enterLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx) = 0;
  virtual void exitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx) = 0;

  virtual void enterLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) = 0;
  virtual void exitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) = 0;

  virtual void enterStatement(JavaParser::StatementContext *ctx) = 0;
  virtual void exitStatement(JavaParser::StatementContext *ctx) = 0;

  virtual void enterSwitchBlock(JavaParser::SwitchBlockContext *ctx) = 0;
  virtual void exitSwitchBlock(JavaParser::SwitchBlockContext *ctx) = 0;

  virtual void enterSwitchLabel(JavaParser::SwitchLabelContext *ctx) = 0;
  virtual void exitSwitchLabel(JavaParser::SwitchLabelContext *ctx) = 0;

  virtual void enterCatchClause(JavaParser::CatchClauseContext *ctx) = 0;
  virtual void exitCatchClause(JavaParser::CatchClauseContext *ctx) = 0;

  virtual void enterCatchType(JavaParser::CatchTypeContext *ctx) = 0;
  virtual void exitCatchType(JavaParser::CatchTypeContext *ctx) = 0;

  virtual void enterFinallyBlock(JavaParser::FinallyBlockContext *ctx) = 0;
  virtual void exitFinallyBlock(JavaParser::FinallyBlockContext *ctx) = 0;

  virtual void enterForControl(JavaParser::ForControlContext *ctx) = 0;
  virtual void exitForControl(JavaParser::ForControlContext *ctx) = 0;

  virtual void enterEnhancedForControl(JavaParser::EnhancedForControlContext *ctx) = 0;
  virtual void exitEnhancedForControl(JavaParser::EnhancedForControlContext *ctx) = 0;

  virtual void enterForInit(JavaParser::ForInitContext *ctx) = 0;
  virtual void exitForInit(JavaParser::ForInitContext *ctx) = 0;

  virtual void enterForUpdate(JavaParser::ForUpdateContext *ctx) = 0;
  virtual void exitForUpdate(JavaParser::ForUpdateContext *ctx) = 0;

  virtual void enterExpressionList(JavaParser::ExpressionListContext *ctx) = 0;
  virtual void exitExpressionList(JavaParser::ExpressionListContext *ctx) = 0;

  virtual void enterStatementExpression(JavaParser::StatementExpressionContext *ctx) = 0;
  virtual void exitStatementExpression(JavaParser::StatementExpressionContext *ctx) = 0;

  virtual void enterParExpression(JavaParser::ParExpressionContext *ctx) = 0;
  virtual void exitParExpression(JavaParser::ParExpressionContext *ctx) = 0;

  virtual void enterExpression(JavaParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(JavaParser::ExpressionContext *ctx) = 0;

  virtual void enterLambdaExpression(JavaParser::LambdaExpressionContext *ctx) = 0;
  virtual void exitLambdaExpression(JavaParser::LambdaExpressionContext *ctx) = 0;

  virtual void enterLambdaParameters(JavaParser::LambdaParametersContext *ctx) = 0;
  virtual void exitLambdaParameters(JavaParser::LambdaParametersContext *ctx) = 0;

  virtual void enterLambdaBody(JavaParser::LambdaBodyContext *ctx) = 0;
  virtual void exitLambdaBody(JavaParser::LambdaBodyContext *ctx) = 0;

  virtual void enterAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) = 0;
  virtual void exitAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) = 0;

  virtual void enterLeftHandSide(JavaParser::LeftHandSideContext *ctx) = 0;
  virtual void exitLeftHandSide(JavaParser::LeftHandSideContext *ctx) = 0;

  virtual void enterAssignmentOperator(JavaParser::AssignmentOperatorContext *ctx) = 0;
  virtual void exitAssignmentOperator(JavaParser::AssignmentOperatorContext *ctx) = 0;

  virtual void enterConditionalExpression(JavaParser::ConditionalExpressionContext *ctx) = 0;
  virtual void exitConditionalExpression(JavaParser::ConditionalExpressionContext *ctx) = 0;

  virtual void enterConditionalOrExpression(JavaParser::ConditionalOrExpressionContext *ctx) = 0;
  virtual void exitConditionalOrExpression(JavaParser::ConditionalOrExpressionContext *ctx) = 0;

  virtual void enterConditionalAndExpression(JavaParser::ConditionalAndExpressionContext *ctx) = 0;
  virtual void exitConditionalAndExpression(JavaParser::ConditionalAndExpressionContext *ctx) = 0;

  virtual void enterInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext *ctx) = 0;
  virtual void exitInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext *ctx) = 0;

  virtual void enterExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext *ctx) = 0;
  virtual void exitExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext *ctx) = 0;

  virtual void enterAndExpression(JavaParser::AndExpressionContext *ctx) = 0;
  virtual void exitAndExpression(JavaParser::AndExpressionContext *ctx) = 0;

  virtual void enterEqualityExpression(JavaParser::EqualityExpressionContext *ctx) = 0;
  virtual void exitEqualityExpression(JavaParser::EqualityExpressionContext *ctx) = 0;

  virtual void enterRelationalExpression(JavaParser::RelationalExpressionContext *ctx) = 0;
  virtual void exitRelationalExpression(JavaParser::RelationalExpressionContext *ctx) = 0;

  virtual void enterShiftExpression(JavaParser::ShiftExpressionContext *ctx) = 0;
  virtual void exitShiftExpression(JavaParser::ShiftExpressionContext *ctx) = 0;

  virtual void enterAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) = 0;
  virtual void exitAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) = 0;

  virtual void enterMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) = 0;
  virtual void exitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) = 0;

  virtual void enterUnaryExpression(JavaParser::UnaryExpressionContext *ctx) = 0;
  virtual void exitUnaryExpression(JavaParser::UnaryExpressionContext *ctx) = 0;

  virtual void enterUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext *ctx) = 0;
  virtual void exitUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext *ctx) = 0;

  virtual void enterCastExpression(JavaParser::CastExpressionContext *ctx) = 0;
  virtual void exitCastExpression(JavaParser::CastExpressionContext *ctx) = 0;

  virtual void enterPostfixExpression(JavaParser::PostfixExpressionContext *ctx) = 0;
  virtual void exitPostfixExpression(JavaParser::PostfixExpressionContext *ctx) = 0;

  virtual void enterPostfixPart(JavaParser::PostfixPartContext *ctx) = 0;
  virtual void exitPostfixPart(JavaParser::PostfixPartContext *ctx) = 0;

  virtual void enterPrimary(JavaParser::PrimaryContext *ctx) = 0;
  virtual void exitPrimary(JavaParser::PrimaryContext *ctx) = 0;

  virtual void enterCreator(JavaParser::CreatorContext *ctx) = 0;
  virtual void exitCreator(JavaParser::CreatorContext *ctx) = 0;

  virtual void enterCreatedName(JavaParser::CreatedNameContext *ctx) = 0;
  virtual void exitCreatedName(JavaParser::CreatedNameContext *ctx) = 0;

  virtual void enterClassCreatorRest(JavaParser::ClassCreatorRestContext *ctx) = 0;
  virtual void exitClassCreatorRest(JavaParser::ClassCreatorRestContext *ctx) = 0;

  virtual void enterArrayCreatorRest(JavaParser::ArrayCreatorRestContext *ctx) = 0;
  virtual void exitArrayCreatorRest(JavaParser::ArrayCreatorRestContext *ctx) = 0;

  virtual void enterArguments(JavaParser::ArgumentsContext *ctx) = 0;
  virtual void exitArguments(JavaParser::ArgumentsContext *ctx) = 0;

  virtual void enterMethodInvocation(JavaParser::MethodInvocationContext *ctx) = 0;
  virtual void exitMethodInvocation(JavaParser::MethodInvocationContext *ctx) = 0;

  virtual void enterFieldAccess(JavaParser::FieldAccessContext *ctx) = 0;
  virtual void exitFieldAccess(JavaParser::FieldAccessContext *ctx) = 0;

  virtual void enterArrayAccess(JavaParser::ArrayAccessContext *ctx) = 0;
  virtual void exitArrayAccess(JavaParser::ArrayAccessContext *ctx) = 0;

  virtual void enterExpressionName(JavaParser::ExpressionNameContext *ctx) = 0;
  virtual void exitExpressionName(JavaParser::ExpressionNameContext *ctx) = 0;

  virtual void enterAmbiguousName(JavaParser::AmbiguousNameContext *ctx) = 0;
  virtual void exitAmbiguousName(JavaParser::AmbiguousNameContext *ctx) = 0;

  virtual void enterLiteral(JavaParser::LiteralContext *ctx) = 0;
  virtual void exitLiteral(JavaParser::LiteralContext *ctx) = 0;

  virtual void enterTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext *ctx) = 0;
  virtual void exitTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext *ctx) = 0;


};

