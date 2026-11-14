#pragma once

#include "jaot/machine.h"

namespace JAOT {
    class RuntimeEmitter {
    public:
        explicit RuntimeEmitter(Machine &machine);

        [[nodiscard]] ValueLocation allocate(std::size_t size) const;
        void print(ValueLocation value, std::string_view name) const;
        void failNullDereference() const;

    private:
        Machine &machine_;
    };
}
