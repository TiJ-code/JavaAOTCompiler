#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace JAOT {

    enum class ValueKind {
        Int32,
        Reference64,
    };

    enum class ValueStorage {
        Eax,
        Rax,
        Ecx,
        Rcx,
        Edx,
        Rdx,
        Stack,
    };

    struct ValueLocation {
        ValueKind kind;
        ValueStorage storage;

        static ValueLocation intInEax() {
            return ValueLocation{ .kind = ValueKind::Int32, .storage = ValueStorage::Eax };
        }

        static ValueLocation intInEcx() {
            return ValueLocation{ .kind = ValueKind::Int32, .storage = ValueStorage::Ecx };
        }

        static ValueLocation intInEdx() {
            return ValueLocation{ .kind = ValueKind::Int32, .storage = ValueStorage::Edx };
        }

        static ValueLocation referenceInRax() {
            return ValueLocation{ .kind = ValueKind::Reference64, .storage = ValueStorage::Rax };
        }

        static ValueLocation referenceInRcx() {
            return ValueLocation{ .kind = ValueKind::Reference64, .storage = ValueStorage::Rcx };
        }

        static ValueLocation referenceInRdx() {
            return ValueLocation{ .kind = ValueKind::Reference64, .storage = ValueStorage::Rdx };
        }

        static ValueLocation onStack(const ValueKind kind) {
            return ValueLocation{ .kind = kind, .storage = ValueStorage::Stack };
        }

        [[nodiscard]] bool isReference() const {
            return kind == ValueKind::Reference64;
        }
    };

    class Machine {
    public:
        explicit Machine(std::ostream &out);

        void beginFunction(std::string_view symbol, int frameSize);

        void endFunction() const;

        int stackDepth() const;

        ValueLocation emitIntConstant(std::int64_t value) const;
        ValueLocation emitBooleanConstant(bool value) const;
        ValueLocation emitNull() const;

        ValueLocation loadFrameSlot(int offset, ValueKind kind) const;

        void storeFrameSlot(int offset, ValueLocation value) const;

        ValueLocation loadMemory(ValueLocation address, int offset, ValueKind kind) const;
        void storeMemory(ValueLocation address, int offset, ValueLocation value) const;

        ValueLocation saveForLater(ValueLocation value) const;

        void storeArgumentInFrame(std::size_t index, int offset, ValueKind kind) const;

        void normalizeBoolean() const;

        void push(ValueLocation value);
        ValueLocation pop(ValueKind kind);

        void call(std::string_view symbol) const;

        void prepareArgument(ValueLocation value, std::size_t index) const;

        void collectArguments(std::size_t count);

        void test(ValueLocation value) const;

        void jump(std::string_view label) const;
        void jumpIfZero(ValueLocation value, std::string_view label) const;
        void jumpIfNotZero(ValueLocation value, std::string_view label) const;

        void label(std::string_view label) const;

        void emitUnaryNot() const;
        void emitBinaryIntOperation(std::string_view operation) const;

        void emitComparison(ValueLocation left, ValueLocation right, std::string_view operation) const;

        void emitDivision(std::string_view methodName, std::size_t labelId) const;

    private:
        void emitCallInstruction(std::string_view symbol) const;

        std::ostream &out_;
        int stackDepth_ = 0;
    };
}
