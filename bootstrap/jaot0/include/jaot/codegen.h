#pragma once

#include "jaot/parser.h"

#include <ostream>

namespace JAOT {
    class CodeGenerator {
    public:
        void generate(const Program &program, std::ostream &out);
    };
} // namespace JAOT
