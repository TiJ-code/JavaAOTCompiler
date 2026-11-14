#include "jaot/runtime_codegen.h"

namespace JAOT {
    RuntimeEmitter::RuntimeEmitter(Machine &machine) : machine_(machine) {
    }

    ValueLocation RuntimeEmitter::allocate(const std::size_t size) const {
        machine_.emitIntConstant(static_cast<std::int64_t>(size));
        machine_.prepareArgument(ValueLocation::intInEax(), 0);
        machine_.call("jaot_allocate");
        return ValueLocation::referenceInRax();
    }

    void RuntimeEmitter::print(const ValueLocation value, const std::string_view name) const {
        if (name == "printChar") {
            if (value.kind != ValueKind::Char8) {
                throw std::runtime_error("printChar requires a char value");
            }
        } else if (name == "printByte") {
            if (value.kind != ValueKind::Int8) {
                throw std::runtime_error("printByte requires a byte value");
            }
        } else if (name == "printShort") {
            if (value.kind != ValueKind::Int16) {
                throw std::runtime_error("printShort requires a short value");
            }
        } else if (name == "printLong") {
            if (value.kind != ValueKind::Int64) {
                throw std::runtime_error("printLong requires a long value");
            }
        } else if (name != "printInt" &&
                   name != "printBool") {
            throw std::runtime_error("unknown runtime print function: " +
                                     std::string(name));
        } else if (value.kind != ValueKind::Int32) {
            throw std::runtime_error("runtime print operation requires an integer value");
        }

        machine_.prepareArgument(value, 0);
        if (name == "printChar") {
            machine_.call("jaot_print_char");
        } else if (name == "printByte") {
            machine_.call("jaot_print_byte");
        } else if (name == "printShort") {
            machine_.call("jaot_print_short");
        } else if (name == "printLong") {
            machine_.call("jaot_print_long");
        } else if (name == "printBool") {
            machine_.call("jaot_print_bool");
        } else {
            machine_.call("jaot_print_int");
        }
    }

    void RuntimeEmitter::failNullDereference() const {
        machine_.call("jaot_null_dereference");
    }
}
