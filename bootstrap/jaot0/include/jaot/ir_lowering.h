#pragma once

#include "jaot/ir.h"

namespace JAOT {
    struct Program;

    class IrLowerer {
    public:
        [[nodiscard]] IR::Program lower(const Program &program) const;
    };
}