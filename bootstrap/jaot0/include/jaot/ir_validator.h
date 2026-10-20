#pragma once

#include "jaot/ir.h"

namespace JAOT {
    class IrValidator {
    public:
        static void validate(const IR::Program &program);
    };
}