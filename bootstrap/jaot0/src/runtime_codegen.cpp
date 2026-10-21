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

    void RuntimeEmitter::print(const ValueLocation value, const bool boolean) const {
        if (value.kind != ValueKind::Int32) {
            throw std::runtime_error("runtime print operation requires an integer value");
        }

        machine_.prepareArgument(value, 0);
        machine_.call(boolean ? "jaot_print_bool" : "jaot_print_int");
    }

    void RuntimeEmitter::failNullDereference() const {
        machine_.call("jaot_null_dereference");
    }
}
