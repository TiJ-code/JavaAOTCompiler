#include "jaot/machine.h"

#include <array>
#include <stdexcept>

namespace JAOT {
    namespace {
        constexpr std::array<const char *, 6> ArgumentRegisters = {
            "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
        };

        constexpr std::array<const char *, 6> ArgumentRegisters32 = {
            "%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"
        };

    }

    Machine::Machine(std::ostream &out) : out_(out) {}

    void Machine::beginFunction(const std::string_view symbol, const int frameSize) {
        out_ << ".text\n";
        out_ << ".globl " << symbol << "\n";
        out_ << ".type " << symbol << ", @function\n";
        out_ << symbol << ":\n";
        out_ << "    pushq %rbp\n";
        out_ << "    movq %rsp, %rbp\n";

        if (frameSize > 0) {
            out_ << "    subq $" << frameSize << ", %rsp\n";
        }

        stackDepth_ = 0;
    }

    void Machine::endFunction() const {
        out_ << "    leave\n";
        out_ << "    ret\n";
    }

    int Machine::stackDepth() const {
        return stackDepth_;
    }

    void Machine::emitRaw(const std::string_view text) const {
        out_ << text;
    }

    ValueLocation Machine::emitIntConstant(const std::int64_t value) const {
        out_ << "    movl $" << value << ", %eax\n";
        return ValueLocation::intInEax();
    }

    ValueLocation Machine::emitBooleanConstant(const bool value) const {
        return emitIntConstant(value ? 1 : 0);
    }

    ValueLocation Machine::emitNull() const {
        out_ << "    xorq %rax, %rax\n";
        return ValueLocation::referenceInRax();
    }

    ValueLocation Machine::loadFrameSlot(const int offset, const ValueKind kind) const {
        if (kind == ValueKind::Reference64) {
            out_ << "    movq " << offset << "(%rbp), %rax\n";
            return ValueLocation::referenceInRax();
        }

        out_ << "    movl " << offset << "(%rbp), %eax\n";
        return ValueLocation::intInEax();
    }

    void Machine::storeFrameSlot(const int offset, const ValueLocation value) const {
        if (value.storage == ValueStorage::Stack) {
            throw std::runtime_error("cannot store a stack-resident value directly into a frame slot");
        }

        if (value.kind == ValueKind::Reference64) {
            out_ << "    movq %rax, ";
        } else {
            out_ << "    movl %eax, ";
        }

        out_ << offset << "(%rbp)\n";
    }

    void Machine::push(const ValueLocation value) {
        if (value.storage == ValueStorage::Stack) {
            throw std::runtime_error("cannot push a value that is already stack-resident");
        }

        /*
         * both integer and reference values occupy one 8 byte temporary
         * stack slot. Integer values are intentionally pushed through %rax
         * while technically only the low 32 bits are semantically significant.
         */
        out_ << "    pushq %rax\n";
        stackDepth_ += 8;
    }

    ValueLocation Machine::pop(const ValueKind kind) {
        out_ << "    popq %rax\n";
        stackDepth_ -= 8;

        if (kind == ValueKind::Reference64) {
            return ValueLocation::referenceInRax();
        }

        return ValueLocation::intInEax();
    }

    void Machine::call(const std::string_view symbol) const {
        emitCallInstruction(symbol);
    }

    void Machine::prepareArgument(const ValueLocation value, const std::size_t index) const {
        if (index >= ArgumentRegisters.size()) {
            throw std::runtime_error("JAOT0 functions support at most six arguments");
        }

        if (value.storage == ValueStorage::Stack) {
            throw std::runtime_error("cannot prepare a stack-resident value as an argument");
        }

        if (value.kind == ValueKind::Reference64) {
            out_ << "    movq %rax, " << ArgumentRegisters[index] << "\n";
        } else {
            out_ << "    movl %eax, " << ArgumentRegisters32[index] << "\n";
        }
    }

    void Machine::collectArguments(const std::size_t count) {
        if (count > ArgumentRegisters.size()) {
            throw std::runtime_error("JAOT0 functions support at most six arguments");
        }

        for (std::size_t i = count; i > 0; --i) {
            out_ << "    popq " << ArgumentRegisters[i - 1] << "\n";
            stackDepth_ -= 8;
        }
    }

    void Machine::test(const ValueLocation value) const {
        if (value.storage == ValueStorage::Stack) {
            throw std::runtime_error("cannot test a stack-resident value directly");
        }

        if (value.kind == ValueKind::Reference64) {
            out_ << "    testq %rax, %rax\n";
        } else {
            out_ << "    testl %eax, %eax\n";
        }
    }

    void Machine::jump(const std::string_view label) const {
        out_ << "    jmp " << label << "\n";
    }

    void Machine::jumpIfZero(const ValueLocation value, const std::string_view label) const {
        test(value);

        out_ << "    je " << label << "\n";
    }

    void Machine::jumpIfNotZero(const ValueLocation value, const std::string_view label) const {
        test(value);

        out_ << "    jne " << label << "\n";
    }

    void Machine::label(const std::string_view label) const {
        out_ << label << ":\n";
    }

    void Machine::emitUnaryNot() const {
        out_ << "    xorl $1, %eax\n";
        out_ << "    andl $1, %eax\n";
    }

    void Machine::emitBinaryIntOperation(const std::string_view operation) const {
        if (operation == "+") {
            out_ << "    addl %ecx, %eax\n";
            return;
        }

        if (operation == "-") {
            out_ << "    subl %ecx, %eax\n";
            return;
        }

        if (operation == "*") {
            out_ << "    imull %ecx, %eax\n";
            return;
        }

        throw std::runtime_error(
            "unknown integer binary operation: " + std::string(operation)
        );
    }

    void Machine::emitComparison(const ValueLocation left, const ValueLocation right, const std::string_view operation) const {
        if (left.storage == ValueStorage::Stack || right.storage == ValueStorage::Stack) {
            throw std::runtime_error("comparison operands must be in registers");
        }

        if (left.kind != right.kind) {
            throw std::runtime_error("comparison operands have incompatible value kinds");
        }

        const char *condition = nullptr;

        if (operation == "==") {
            condition = "e";
        } else if (operation == "!=") {
            condition = "ne";
        } else if (operation == "<") {
            condition = "l";
        } else if (operation == ">") {
            condition = "g";
        } else if (operation == "<=") {
            condition = "le";
        } else if (operation == ">=") {
            condition = "ge";
        } else {
            throw std::runtime_error(
                "unknown comparison operation: " + std::string(operation)
            );
        }

        if (left.kind == ValueKind::Reference64) {
            out_ << "    cmpq %rcx, %rax\n";
        } else {
            out_ << "    cmpl %ecx, %eax\n";
        }

        out_ << "    set" << condition << "  %al\n";
        out_ << "    movzbl %al, %eax\n";
    }

    void Machine::emitDivision(const std::string_view methodName, const std::size_t labelId) const {
        const std::string base = ".Ljaot_" + std::string(methodName) + "_division_" + std::to_string(labelId);

        const std::string normalLabel = base + "_normal";
        const std::string overflowLabel = base + "_overflow";
        const std::string zeroLabel = base + "_zero";
        const std::string doneLabel = base + "_done";

        out_ << "    testl %ecx, %ecx\n";
        out_ << "    je " << zeroLabel << "\n";

        out_ << "    cmpl $-1, %ecx\n";
        out_ << "    jne " << normalLabel << "\n";

        out_ << "    cmpl $-2147483648, %eax\n";
        out_ << "    je " << overflowLabel << "\n";

        out_ << normalLabel << ":\n";
        out_ << "    cltd\n";
        out_ << "    idivl %ecx\n";
        out_ << "    jmp " << doneLabel << "\n";

        out_ << overflowLabel << ":\n";
        out_ << "    movl $-2147483648, %eax\n";
        out_ << "    jmp " << doneLabel << "\n";

        out_ << zeroLabel << ":\n";
        out_ << "    ud2\n";

        out_ << doneLabel << ":\n";
    }

    void Machine::emitCallInstruction(const std::string_view symbol) const {
        const bool needsPadding = stackDepth_ % 16 != 0;

        if (needsPadding) {
            out_ << "    subq $8, %rsp\n";
        }

        out_ << "    call " << symbol << "\n";

        if (needsPadding) {
            out_ << "    addq $8, %rsp\n";
        }
    }
}
