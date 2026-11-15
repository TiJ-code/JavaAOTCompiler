#pragma once

#include "jaot/machine.h"

namespace JAOT {
    class RuntimeEmitter {
    public:
        explicit RuntimeEmitter(Machine &machine);

        [[nodiscard]] ValueLocation allocate(std::size_t size) const;
        void print(ValueLocation value, std::string_view name) const;
        void failNullDereference() const;

        [[nodiscard]] ValueLocation allocateArray(ValueLocation length, std::size_t elementSize) const;
        [[nodiscard]] ValueLocation arrayElement(ValueLocation array, ValueLocation index, std::size_t elementSize) const;

    private:
        Machine &machine_;
    };
}
