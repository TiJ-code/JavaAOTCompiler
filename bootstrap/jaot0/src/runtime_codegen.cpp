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
        if (name == "printShort") {
            if (value.kind != ValueKind::Int16) {
                throw std::runtime_error("printShort requires a short value");
            }
        } else if (name == "printLong") {
            if (value.kind != ValueKind::Int64) {
                throw std::runtime_error("printLong requires a long value");
            }
        } else if (value.kind != ValueKind::Int32) {
            throw std::runtime_error("runtime print operation requires an integer value");
        }

        machine_.prepareArgument(value, 0);
        machine_.call(name == "printBool" ? "jaot_print_bool" :
                      name == "printLong" ? "jaot_print_long" :
                      name == "printInt" ? "jaot_print_int" :
                      name == "printShort" ? "jaot_print_short" : "jaot_print_char");
    }

    void RuntimeEmitter::failNullDereference() const {
        machine_.call("jaot_null_dereference");
    }
}
