#pragma once

#include "jaot/ir.h"

#include <ostream>

namespace JAOT {
    class CodeGenerator {
    public:
        void generate(const IR::Program &program, std::ostream &out);
    };
} // namespace JAOT
