#include "jaot/expression_codegen.h"

#include <stdexcept>
#include <utility>

namespace JAOT {
    ExpressionLowerer::ExpressionLowerer(
        Machine &machine,
        RuntimeEmitter &runtime,
        const FrameLayout &frame,
        const std::unordered_map<std::string, MethodSignature> &methods,
        const std::unordered_map<std::string, IR::Class> &classes,
        std::string methodName
    )
        : machine_(machine),
          runtime_(runtime),
          frame_(frame),
          methods_(methods),
          classes_(classes),
          methodName_(std::move(methodName)) {
    }

    ValueKind ExpressionLowerer::valueKind(const TypeRef &type) const {
        return type.kind == Type::Reference
                   ? ValueKind::Reference64
                   : ValueKind::Int32;
    }

    std::string ExpressionLowerer::makeLabel(std::string_view prefix) {
        return ".Ljaot_" + methodName_ + "_" +
               std::string(prefix) + "_" +
               std::to_string(labelCounter_++);
    }

    ValueLocation ExpressionLowerer::lower(
        const IR::Expr &expression
    ) {
        switch (expression.kind) {
            case IR::ExprKind::Integer:
                return machine_.emitIntConstant(expression.integer);

            case IR::ExprKind::Boolean:
                return machine_.emitBooleanConstant(expression.boolean);

            case IR::ExprKind::Null:
                return machine_.emitNull();

            case IR::ExprKind::NewObject: {
                const auto found = classes_.find(expression.className);
                if (found == classes_.end()) {
                    throw std::runtime_error(
                        "unknown class: " + expression.className);
                }

                return runtime_.allocate(found->second.size);
            }

            case IR::ExprKind::Local:
                return machine_.loadFrameSlot(
                    frame_.offset(expression.local),
                    valueKind(expression.type)
                );

            case IR::ExprKind::FieldAccess:
                return lowerFieldAccess(expression);

            case IR::ExprKind::Unary:
                return lowerUnary(expression);

            case IR::ExprKind::Binary:
                return lowerBinary(expression);

            case IR::ExprKind::Call:
                return lowerCall(expression);
        }

        throw std::runtime_error("unknown IR expression kind");
    }

    ValueLocation ExpressionLowerer::lowerFieldAccess(
        const IR::Expr &expression
    ) {
        if (!expression.left) {
            throw std::runtime_error("incomplete IR field access");
        }

        const ValueLocation receiver = lower(*expression.left);
        const std::string nonNullLabel = makeLabel("nonnull");

        machine_.jumpIfNotZero(receiver, nonNullLabel);
        runtime_.failNullDereference();
        machine_.label(nonNullLabel);

        machine_.emitRaw(
            expression.type.kind == Type::Reference
                ? "    movq "
                : "    movl "
        );
        machine_.emitRaw(
            std::to_string(expression.fieldOffset) +
            "(%rax), " +
            (expression.type.kind == Type::Reference
                 ? "%rax\n"
                 : "%eax\n")
        );

        return expression.type.kind == Type::Reference
                   ? ValueLocation::referenceInRax()
                   : ValueLocation::intInEax();
    }

    ValueLocation ExpressionLowerer::lowerUnary(
        const IR::Expr &expression
    ) {
        if (!expression.operand) {
            throw std::runtime_error("incomplete IR unary expression");
        }

        lower(*expression.operand);

        if (expression.unaryOperator == "!") {
            machine_.emitUnaryNot();
            return ValueLocation::intInEax();
        }

        throw std::runtime_error(
            "unknown IR unary operator");
    }

    ValueLocation ExpressionLowerer::lowerBinary(
        const IR::Expr &expression
    ) {
        if (!expression.left || !expression.right) {
            throw std::runtime_error(
                "incomplete IR binary expression");
        }

        if (expression.binaryOperator == "&&") {
            const std::string falseLabel = makeLabel("logical_and_false");
            const std::string doneLabel = makeLabel("logical_and_done");

            const ValueLocation left = lower(*expression.left);
            machine_.jumpIfZero(left, falseLabel);

            lower(*expression.right);
            machine_.emitRaw("    testl %eax, %eax\n");
            machine_.emitRaw("    setne %al\n");
            machine_.emitRaw("    movzbl %al, %eax\n");
            machine_.jump(doneLabel);

            machine_.label(falseLabel);
            machine_.emitBooleanConstant(false);
            machine_.label(doneLabel);
            return ValueLocation::intInEax();
        }

        if (expression.binaryOperator == "||") {
            const std::string trueLabel = makeLabel("logical_or_true");
            const std::string doneLabel = makeLabel("logical_or_done");

            const ValueLocation left = lower(*expression.left);
            machine_.jumpIfNotZero(left, trueLabel);

            lower(*expression.right);
            machine_.emitRaw("    testl %eax, %eax\n");
            machine_.emitRaw("    setne %al\n");
            machine_.emitRaw("    movzbl %al, %eax\n");
            machine_.jump(doneLabel);

            machine_.label(trueLabel);
            machine_.emitBooleanConstant(true);
            machine_.label(doneLabel);
            return ValueLocation::intInEax();
        }

        const ValueLocation left = lower(*expression.left);
        machine_.push(left);

        const ValueLocation right = lower(*expression.right);
        if (left.kind == ValueKind::Reference64) {
            machine_.emitRaw("    movq %rax, %rcx\n");
        } else {
            machine_.emitRaw("    movl %eax, %ecx\n");
        }

        const ValueLocation restoredLeft = machine_.pop(left.kind);

        if (expression.binaryOperator == "+" ||
            expression.binaryOperator == "-" ||
            expression.binaryOperator == "*") {
            machine_.emitBinaryIntOperation(expression.binaryOperator);
            return ValueLocation::intInEax();
        }

        if (expression.binaryOperator == "/") {
            machine_.emitDivision(methodName_, labelCounter_++);
            return ValueLocation::intInEax();
        }

        machine_.emitComparison(
            restoredLeft,
            right,
            expression.binaryOperator
        );
        return ValueLocation::intInEax();
    }

    ValueLocation ExpressionLowerer::lowerCall(
        const IR::Expr &expression
    ) {
        if (expression.callee.ownerClass == "<runtime>") {
            if (expression.arguments.size() != 1) {
                throw std::runtime_error(
                    expression.callee.name + " expects one argument");
            }

            const ValueLocation argument =
                    lower(*expression.arguments[0]);

            runtime_.print(
                argument,
                expression.callee.name == "printBool"
            );
            return ValueLocation::intInEax();
        }

        const auto method = methods_.find(
            expression.callee.ownerClass + "." +
            expression.callee.name
        );
        if (method == methods_.end()) {
            throw std::runtime_error(
                "unknown function: " +
                expression.callee.ownerClass + "." +
                expression.callee.name
            );
        }

        if (expression.arguments.size() !=
            method->second.parameterTypes.size()) {
            throw std::runtime_error(
                "function " + expression.callee.name + " expects " +
                std::to_string(method->second.parameterTypes.size()) +
                " arguments"
            );
        }

        if (expression.receiver) {
            throw std::runtime_error(
                "instance method calls are not supported by JAOT0"
            );
        }

        if (expression.arguments.size() > 6) {
            throw std::runtime_error(
                "JAOT0 functions support at most six arguments"
            );
        }

        for (const auto &argument: expression.arguments) {
            machine_.push(lower(*argument));
        }

        machine_.collectArguments(expression.arguments.size());
        machine_.call("jaot_" + methodSymbol(method->second));

        return expression.type.kind == Type::Reference
                   ? ValueLocation::referenceInRax()
                   : ValueLocation::intInEax();
    }
} // namespace JAOT
