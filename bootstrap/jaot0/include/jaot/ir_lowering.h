#pragma once

#include "jaot/ir.h"

namespace JAOT {
    class IrLowerer {
    public:
        IR::Program lower(const Program &program) const;
    };
}