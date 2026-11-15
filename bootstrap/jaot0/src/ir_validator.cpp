#include "jaot/ir_validator.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace JAOT {
    namespace {
        [[noreturn]] void invalid(const std::string &message) {
            throw std::runtime_error("invalid IR: " + message);
        }

        std::size_t typeSize(const TypeRef &type) {
            switch (type.kind) {
                case Type::Char:
                case Type::Byte:
                    return 1;
                case Type::Short:
                    return 2;
                case Type::Int:
                case Type::Boolean:
                    return 4;
                case Type::Long:
                case Type::Reference:
                case Type::Array:
                    return 8;
                case Type::Void:
                    invalid("void type has no storage size");
            }

            invalid("unknown type");
        }

        void validateExpression(
            const IR::Expr &expression,
            const std::unordered_map<IR::LocalId, TypeRef> &locals,
            const std::unordered_map<std::string, IR::Class> &classes,
            const std::unordered_map<std::string, MethodSignature> &methods
        ) {
            switch (expression.kind) {
                case IR::ExprKind::Character:
                    if (expression.type.kind != Type::Char) {
                        invalid("character expression has non-char type");
                    }
                    return;

                case IR::ExprKind::ByteInteger:
                    if (expression.type.kind != Type::Byte) {
                        invalid("byte integer expression has non-byte type");
                    }
                    return;

                case IR::ExprKind::Integer:
                    if (expression.type.kind != Type::Int) {
                        invalid("integer expression has non-int type");
                    }
                    return;

                case IR::ExprKind::ShortInteger:
                    if (expression.type.kind != Type::Short) {
                        invalid("short integer expression has non-short type");
                    }
                    return;

                case IR::ExprKind::LongInteger:
                    if (expression.type.kind != Type::Long) {
                        invalid("long integer expression has non-long type");
                    }
                    return;

                case IR::ExprKind::Boolean:
                    if (expression.type.kind != Type::Boolean) {
                        invalid("boolean expression has non-boolean type");
                    }
                    return;

                case IR::ExprKind::Null:
                    if (expression.type.kind != Type::Reference) {
                        invalid("null expression has non-null type");
                    }
                    return;

                case IR::ExprKind::NewObject:
                    if (expression.type.kind != Type::Reference) {
                        invalid("object allocation is not a reference");
                    }
                    if (!classes.contains(expression.className)) {
                        invalid("allocation references unknown class " + expression.className);
                    }
                    return;

                case IR::ExprKind::NewArray:
                    if (expression.type.kind != Type::Array ||
                        !expression.type.elementType ||
                        !sameType(
                            *expression.type.elementType,
                            expression.elementType
                        )) {
                        invalid("array allocation has incorrect type");
                    }

                    if (!expression.right) {
                        invalid("array allocation has no length");
                    }

                    validateExpression(
                        *expression.right,
                        locals,
                        classes,
                        methods
                    );

                    if (expression.right->type.kind != Type::Int) {
                        invalid("array length is not an int");
                    }

                    if (expression.elementSize != typeSize(expression.elementType)) {
                        invalid("array allocation has incorrect element size");
                    }

                    if (expression.elementType.kind == Type::Reference &&
                        !expression.elementType.className.empty() &&
                        !classes.contains(expression.elementType.className)) {
                        invalid("array allocation references unknown class");
                    }
                    return;

                case IR::ExprKind::Local: {
                    const auto found = locals.find(expression.local);

                    if (found == locals.end()) {
                        invalid("expression references unknown local");
                    }

                    if (!sameType(expression.type, found->second)) {
                        invalid("local expression has incorrect type");
                    }

                    return;
                }

                case IR::ExprKind::FieldAccess: {
                    if (!expression.left) {
                        invalid("field access has no receiver");
                    }

                    validateExpression(*expression.left, locals, classes, methods);

                    if (expression.left->type.kind != Type::Reference) {
                        invalid("field receiver is not a reference");
                    }

                    const auto classFound = classes.find(expression.left->type.className);

                    if (classFound == classes.end()) {
                        invalid("field receiver has unknown class");
                    }

                    const auto field = std::ranges::find_if(
                        classFound->second.fields,
                        [&](const IR::Field &candidate) {
                             return candidate.offset == expression.fieldOffset;
                        }
                    );

                    if (field == classFound->second.fields.end()) {
                        invalid("field offset is not present in receiver class");
                    }

                    if (!sameType(expression.type, field->type)) {
                        invalid("field access has incorrect type");
                    }

                    return;
                }

                case IR::ExprKind::ArrayAccess:
                    if (!expression.left || !expression.right) {
                        invalid("array access is incomplete");
                    }

                    validateExpression(
                        *expression.left,
                        locals,
                        classes,
                        methods
                    );
                    validateExpression(
                        *expression.right,
                        locals,
                        classes,
                        methods
                    );

                    if (expression.left->type.kind != Type::Array ||
                        !expression.left->type.elementType) {
                        invalid("array access receiver is not an array");
                    }

                    if (expression.right->type.kind != Type::Int) {
                        invalid("array index is not an int");
                    }

                    if (!sameType(
                            expression.type,
                            *expression.left->type.elementType
                        )) {
                        invalid("array access has incorrect element type");
                    }

                    if (expression.elementSize != typeSize(expression.type)) {
                        invalid("array access has incorrect element size");
                    }
                    return;

                case IR::ExprKind::Unary: {
                    if (!expression.operand) {
                        invalid("unary expression has no operand");
                    }

                    validateExpression(*expression.operand, locals, classes, methods);

                    if (expression.unaryOperator == "!" &&
                        (expression.operand->type.kind != Type::Boolean ||
                         expression.type.kind != Type::Boolean)) {
                        invalid("logical negation requires boolean operands");
                    }

                    return;
                }

                case IR::ExprKind::Binary: {
                    if (!expression.left || !expression.right) {
                        invalid("binary expression is incomplete");
                    }

                    validateExpression(*expression.left, locals, classes, methods);
                    validateExpression(*expression.right, locals, classes, methods);

                    if (expression.left->type.kind != expression.right->type.kind) {
                        invalid("binary operands have different types");
                    }

                    return;
                }

                case IR::ExprKind::Call: {
                    for (const auto &argument : expression.arguments) {
                        validateExpression(*argument, locals, classes, methods);
                    }

                    if (expression.receiver) {
                        validateExpression(*expression.receiver, locals, classes, methods);

                        if (expression.receiver->type.kind != Type::Reference) {
                            invalid("call receiver is not a reference");
                        }
                    }

                    if (expression.callee.ownerClass != "<runtime>") {
                        const auto found = methods.find(
                            expression.callee.ownerClass + "." + expression.callee.name
                        );

                        if (found == methods.end()) {
                            invalid("call references unknown method");
                        }

                        if (found->second.parameterTypes.size() != expression.arguments.size()) {
                            invalid("call argument count does not match method");
                        }

                        for (std::size_t i = 0; i < expression.arguments.size(); ++i) {
                            if (!isAssignable(expression.arguments[i]->type, found->second.parameterTypes[i])) {
                                invalid("call argument type does not match method");
                            }
                        }
                    }

                    if (!sameType(expression.type, expression.callee.returnType)) {
                        invalid("call result type does not match signature");
                    }

                    return;
                }
            }

            invalid("unknown expression kind");
        }
    }

    void IrValidator::validate(const IR::Program &program) {
        std::unordered_map<std::string, IR::Class> classes;
        std::unordered_map<std::string, MethodSignature> methods;

        for (const IR::Class &classDeclaration : program.classes) {
            if (!classes.emplace(classDeclaration.name, classDeclaration).second) {
                invalid("duplicate class " + classDeclaration.name);
            }

            std::size_t expectedSize = 0;
            for (const IR::Field &field : classDeclaration.fields) {
                if (
                    field.size != 1 &&
                    field.size != 2 &&
                    field.size != 4 &&
                    field.size != 8
                ) {
                    invalid("field has invalid size");
                }
                expectedSize += field.size;
            }

            if (expectedSize != classDeclaration.size) {
                invalid("class size does not match field layout");
            }
        }

        for (const IR::Method &method : program.methods) {
            const std::string key = method.signature.ownerClass + "." + method.signature.name;

            if (!methods.emplace(key, method.signature).second) {
                invalid("duplicate method " + key);
            }

            std::unordered_map<IR::LocalId, TypeRef> locals;

            for (const IR::Local &local : method.locals) {
                if (!locals.emplace(local.id, local.type).second) {
                    invalid("duplicate local id");
                }
            }

            for (IR::LocalId parameter : method.parameters) {
                if (!locals.contains(parameter)) {
                    invalid("method parameter references unknown local");
                }
            }

            std::unordered_set<IR::BlockId> blockIds;

            for (const IR::BasicBlock &block : method.blocks) {
                if (!blockIds.insert(block.id).second) {
                    invalid("duplicate basic block id");
                }

                const IR::Terminator &terminator = block.terminator;

                switch (terminator.kind) {
                    case IR::TerminatorKind::Jump:
                        break;

                    case IR::TerminatorKind::Branch:
                        if (!terminator.condition) {
                            invalid("branch has no condition");
                        }

                        validateExpression(*terminator.condition, locals, classes, methods);

                        if (terminator.condition->type.kind != Type::Boolean) {
                            invalid("branch condition is not boolean");
                        }

                        break;

                    case IR::TerminatorKind::Return:
                        if (terminator.expression) {
                            validateExpression(*terminator.expression, locals, classes, methods);

                            if (!isAssignable(terminator.expression->type, method.signature.returnType)) {
                                invalid("return type does not match method");
                            }
                        } else if (method.signature.returnType.kind != Type::Void) {
                            invalid("non-void method has empty return");
                        }
                        break;
                }

                for (const IR::Statement &statement : block.statements) {
                    if (statement.expression) {
                        validateExpression(*statement.expression, locals, classes, methods);
                    }

                    if (statement.targetExpression) {
                        validateExpression(*statement.targetExpression, locals, classes, methods);
                    }

                    if (statement.kind == IR::StatementKind::AssignArray) {
                        if (!statement.targetExpression ||
                            statement.targetExpression->kind != IR::ExprKind::ArrayAccess ||
                            !statement.expression) {
                            invalid("array assignment is incomplete");
                        }

                        if (!isAssignable(
                                statement.expression->type,
                                statement.targetExpression->type
                            )) {
                            invalid("array assignment type does not match");
                        }
                    }
                }
            }
        }
    }
}
