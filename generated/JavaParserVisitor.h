
// Generated from grammar/JavaParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by JavaParser.
 */
class  JavaParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by JavaParser.
   */
    virtual std::any visitCompilationUnit(JavaParser::CompilationUnitContext *context) = 0;

    virtual std::any visitPackageDeclaration(JavaParser::PackageDeclarationContext *context) = 0;

    virtual std::any visitImportDeclaration(JavaParser::ImportDeclarationContext *context) = 0;

    virtual std::any visitQualifiedName(JavaParser::QualifiedNameContext *context) = 0;

    virtual std::any visitAnnotation(JavaParser::AnnotationContext *context) = 0;

    virtual std::any visitElementValue(JavaParser::ElementValueContext *context) = 0;

    virtual std::any visitTypeDeclaration(JavaParser::TypeDeclarationContext *context) = 0;

    virtual std::any visitClassDeclaration(JavaParser::ClassDeclarationContext *context) = 0;

    virtual std::any visitClassModifier(JavaParser::ClassModifierContext *context) = 0;

    virtual std::any visitSuperclass(JavaParser::SuperclassContext *context) = 0;

    virtual std::any visitSuperinterfaces(JavaParser::SuperinterfacesContext *context) = 0;

    virtual std::any visitInterfaceDeclaration(JavaParser::InterfaceDeclarationContext *context) = 0;

    virtual std::any visitEnumDeclaration(JavaParser::EnumDeclarationContext *context) = 0;

    virtual std::any visitRecordDeclaration(JavaParser::RecordDeclarationContext *context) = 0;

    virtual std::any visitRecordComponentList(JavaParser::RecordComponentListContext *context) = 0;

    virtual std::any visitRecordComponent(JavaParser::RecordComponentContext *context) = 0;

    virtual std::any visitInterfaceBody(JavaParser::InterfaceBodyContext *context) = 0;

    virtual std::any visitInterfaceBodyDeclaration(JavaParser::InterfaceBodyDeclarationContext *context) = 0;

    virtual std::any visitEnumBody(JavaParser::EnumBodyContext *context) = 0;

    virtual std::any visitEnumConstantList(JavaParser::EnumConstantListContext *context) = 0;

    virtual std::any visitEnumConstant(JavaParser::EnumConstantContext *context) = 0;

    virtual std::any visitClassBody(JavaParser::ClassBodyContext *context) = 0;

    virtual std::any visitClassBodyDeclaration(JavaParser::ClassBodyDeclarationContext *context) = 0;

    virtual std::any visitMemberDeclaration(JavaParser::MemberDeclarationContext *context) = 0;

    virtual std::any visitModifier(JavaParser::ModifierContext *context) = 0;

    virtual std::any visitFieldDeclaration(JavaParser::FieldDeclarationContext *context) = 0;

    virtual std::any visitVariableDeclarators(JavaParser::VariableDeclaratorsContext *context) = 0;

    virtual std::any visitVariableDeclarator(JavaParser::VariableDeclaratorContext *context) = 0;

    virtual std::any visitVariableDeclaratorId(JavaParser::VariableDeclaratorIdContext *context) = 0;

    virtual std::any visitVariableInitializer(JavaParser::VariableInitializerContext *context) = 0;

    virtual std::any visitVariableModifier(JavaParser::VariableModifierContext *context) = 0;

    virtual std::any visitArrayInitializer(JavaParser::ArrayInitializerContext *context) = 0;

    virtual std::any visitMethodDeclaration(JavaParser::MethodDeclarationContext *context) = 0;

    virtual std::any visitConstructorDeclaration(JavaParser::ConstructorDeclarationContext *context) = 0;

    virtual std::any visitConstructorModifier(JavaParser::ConstructorModifierContext *context) = 0;

    virtual std::any visitThrows_(JavaParser::Throws_Context *context) = 0;

    virtual std::any visitQualifiedNameList(JavaParser::QualifiedNameListContext *context) = 0;

    virtual std::any visitMethodBody(JavaParser::MethodBodyContext *context) = 0;

    virtual std::any visitFormalParameters(JavaParser::FormalParametersContext *context) = 0;

    virtual std::any visitFormalParameterList(JavaParser::FormalParameterListContext *context) = 0;

    virtual std::any visitFormalParameter(JavaParser::FormalParameterContext *context) = 0;

    virtual std::any visitTypeType(JavaParser::TypeTypeContext *context) = 0;

    virtual std::any visitPrimitiveType(JavaParser::PrimitiveTypeContext *context) = 0;

    virtual std::any visitClassOrInterfaceType(JavaParser::ClassOrInterfaceTypeContext *context) = 0;

    virtual std::any visitTypeArguments(JavaParser::TypeArgumentsContext *context) = 0;

    virtual std::any visitTypeArgument(JavaParser::TypeArgumentContext *context) = 0;

    virtual std::any visitTypeParameters(JavaParser::TypeParametersContext *context) = 0;

    virtual std::any visitTypeParameter(JavaParser::TypeParameterContext *context) = 0;

    virtual std::any visitTypeTypeOrVoid(JavaParser::TypeTypeOrVoidContext *context) = 0;

    virtual std::any visitTypeList(JavaParser::TypeListContext *context) = 0;

    virtual std::any visitDims(JavaParser::DimsContext *context) = 0;

    virtual std::any visitBlock(JavaParser::BlockContext *context) = 0;

    virtual std::any visitBlockStatement(JavaParser::BlockStatementContext *context) = 0;

    virtual std::any visitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *context) = 0;

    virtual std::any visitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *context) = 0;

    virtual std::any visitStatement(JavaParser::StatementContext *context) = 0;

    virtual std::any visitSwitchBlock(JavaParser::SwitchBlockContext *context) = 0;

    virtual std::any visitSwitchLabel(JavaParser::SwitchLabelContext *context) = 0;

    virtual std::any visitCatchClause(JavaParser::CatchClauseContext *context) = 0;

    virtual std::any visitCatchType(JavaParser::CatchTypeContext *context) = 0;

    virtual std::any visitFinallyBlock(JavaParser::FinallyBlockContext *context) = 0;

    virtual std::any visitForControl(JavaParser::ForControlContext *context) = 0;

    virtual std::any visitEnhancedForControl(JavaParser::EnhancedForControlContext *context) = 0;

    virtual std::any visitForInit(JavaParser::ForInitContext *context) = 0;

    virtual std::any visitForUpdate(JavaParser::ForUpdateContext *context) = 0;

    virtual std::any visitExpressionList(JavaParser::ExpressionListContext *context) = 0;

    virtual std::any visitStatementExpression(JavaParser::StatementExpressionContext *context) = 0;

    virtual std::any visitParExpression(JavaParser::ParExpressionContext *context) = 0;

    virtual std::any visitExpression(JavaParser::ExpressionContext *context) = 0;

    virtual std::any visitLambdaExpression(JavaParser::LambdaExpressionContext *context) = 0;

    virtual std::any visitLambdaParameters(JavaParser::LambdaParametersContext *context) = 0;

    virtual std::any visitLambdaBody(JavaParser::LambdaBodyContext *context) = 0;

    virtual std::any visitAssignmentExpression(JavaParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitLeftHandSide(JavaParser::LeftHandSideContext *context) = 0;

    virtual std::any visitAssignmentOperator(JavaParser::AssignmentOperatorContext *context) = 0;

    virtual std::any visitConditionalExpression(JavaParser::ConditionalExpressionContext *context) = 0;

    virtual std::any visitConditionalOrExpression(JavaParser::ConditionalOrExpressionContext *context) = 0;

    virtual std::any visitConditionalAndExpression(JavaParser::ConditionalAndExpressionContext *context) = 0;

    virtual std::any visitInclusiveOrExpression(JavaParser::InclusiveOrExpressionContext *context) = 0;

    virtual std::any visitExclusiveOrExpression(JavaParser::ExclusiveOrExpressionContext *context) = 0;

    virtual std::any visitAndExpression(JavaParser::AndExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(JavaParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(JavaParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitPattern(JavaParser::PatternContext *context) = 0;

    virtual std::any visitShiftExpression(JavaParser::ShiftExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(JavaParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitUnaryExpression(JavaParser::UnaryExpressionContext *context) = 0;

    virtual std::any visitUnaryExpressionNotPlusMinus(JavaParser::UnaryExpressionNotPlusMinusContext *context) = 0;

    virtual std::any visitCastExpression(JavaParser::CastExpressionContext *context) = 0;

    virtual std::any visitPostfixExpression(JavaParser::PostfixExpressionContext *context) = 0;

    virtual std::any visitPostfixPart(JavaParser::PostfixPartContext *context) = 0;

    virtual std::any visitPrimary(JavaParser::PrimaryContext *context) = 0;

    virtual std::any visitCreator(JavaParser::CreatorContext *context) = 0;

    virtual std::any visitCreatedName(JavaParser::CreatedNameContext *context) = 0;

    virtual std::any visitClassCreatorRest(JavaParser::ClassCreatorRestContext *context) = 0;

    virtual std::any visitArrayCreatorRest(JavaParser::ArrayCreatorRestContext *context) = 0;

    virtual std::any visitArguments(JavaParser::ArgumentsContext *context) = 0;

    virtual std::any visitMethodInvocation(JavaParser::MethodInvocationContext *context) = 0;

    virtual std::any visitFieldAccess(JavaParser::FieldAccessContext *context) = 0;

    virtual std::any visitArrayAccess(JavaParser::ArrayAccessContext *context) = 0;

    virtual std::any visitExpressionName(JavaParser::ExpressionNameContext *context) = 0;

    virtual std::any visitAmbiguousName(JavaParser::AmbiguousNameContext *context) = 0;

    virtual std::any visitLiteral(JavaParser::LiteralContext *context) = 0;

    virtual std::any visitTypeArgumentsOrDiamond(JavaParser::TypeArgumentsOrDiamondContext *context) = 0;


};

