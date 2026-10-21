#pragma once

#include "jaot/machine.h"

namespace JAOT {
    class RuntimeEmitter {
    public:
        explicit RuntimeEmitter(Machine &machine);

        [[nodiscard]] ValueLocation allocate(std::size_t size) const;
        void print(ValueLocation value, bool boolean) const;
        void failNullDereference() const;

    private:
        Machine &machine_;
    };
}
