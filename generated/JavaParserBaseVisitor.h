
// Generated from grammar/JavaParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaParserVisitor.h"


/**
 * This class provides an empty implementation of JavaParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  JavaParserBaseVisitor : public JavaParserVisitor {
public:

  virtual std::any visitCompilationUnit(JavaParser::CompilationUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackageDeclaration(JavaParser::PackageDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportDeclaration(JavaParser::ImportDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQualifiedName(JavaParser::QualifiedNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnnotation(JavaParser::AnnotationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementValue(JavaParser::ElementValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeDeclaration(JavaParser::TypeDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassModifier(JavaParser::ClassModifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSuperclass(JavaParser::SuperclassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSuperinterfaces(JavaParser::SuperinterfacesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterfaceDeclaration(JavaParser::InterfaceDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumDeclaration(JavaParser::EnumDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRecordDeclaration(JavaParser::RecordDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRecordComponentList(JavaParser::RecordComponentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRecordComponent(JavaParser::RecordComponentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterfaceBody(JavaParser::InterfaceBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumBody(JavaParser::EnumBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumConstantList(JavaParser::EnumConstantListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumConstant(JavaParser::EnumConstantContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassBody(JavaParser::ClassBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemberDeclaration(JavaParser::MemberDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModifier(JavaParser::ModifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldDeclaration(JavaParser::FieldDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarators(JavaParser::VariableDeclaratorsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarator(JavaParser::VariableDeclaratorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableInitializer(JavaParser::VariableInitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableModifier(JavaParser::VariableModifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayInitializer(JavaParser::ArrayInitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstructorDeclaration(JavaParser::ConstructorDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstructorModifier(JavaParser::ConstructorModifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThrows_(JavaParser::Throws_Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQualifiedNameList(JavaParser::QualifiedNameListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodBody(JavaParser::MethodBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameters(JavaParser::FormalParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameterList(JavaParser::FormalParameterListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameter(JavaParser::FormalParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeType(JavaParser::TypeTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimitiveType(JavaParser::PrimitiveTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeArguments(JavaParser::TypeArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeArgument(JavaParser::TypeArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeParameters(JavaParser::TypeParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeParameter(JavaParser::TypeParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeList(JavaParser::TypeListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDims(JavaParser::DimsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(JavaParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStatement(JavaParser::BlockStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(JavaParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchBlock(JavaParser::SwitchBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchLabel(JavaParser::SwitchLabelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchClause(JavaParser::CatchClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchType(JavaParser::CatchTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFinallyBlock(JavaParser::FinallyBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForControl(JavaParser::ForControlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnhancedForControl(JavaParser::EnhancedForControlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInit(JavaParser::ForInitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForUpdate(JavaParser::ForUpdateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionList(JavaParser::ExpressionListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatementExpression(JavaParser::StatementExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParExpression(JavaParser::ParExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(JavaParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLambdaExpression(JavaParser::LambdaExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLambdaParameters(JavaParser::LambdaParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLambdaBody(JavaParser::LambdaBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLeftHandSide(JavaParser::LeftHandSideContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperator(JavaParser::AssignmentOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalExpression(JavaParser::ConditionalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalOrExpression(JavaParser::ConditionalOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalAndExpression(JavaParser::ConditionalAndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAndExpression(JavaParser::AndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExpression(JavaParser::EqualityExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExpression(JavaParser::RelationalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPattern(JavaParser::PatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftExpression(JavaParser::ShiftExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpression(JavaParser::UnaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCastExpression(JavaParser::CastExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostfixExpression(JavaParser::PostfixExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostfixPart(JavaParser::PostfixPartContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary(JavaParser::PrimaryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCreator(JavaParser::CreatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCreatedName(JavaParser::CreatedNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassCreatorRest(JavaParser::ClassCreatorRestContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayCreatorRest(JavaParser::ArrayCreatorRestContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArguments(JavaParser::ArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodInvocation(JavaParser::MethodInvocationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldAccess(JavaParser::FieldAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayAccess(JavaParser::ArrayAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionName(JavaParser::ExpressionNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAmbiguousName(JavaParser::AmbiguousNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(JavaParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext *ctx) override {
    return visitChildren(ctx);
  }


};

