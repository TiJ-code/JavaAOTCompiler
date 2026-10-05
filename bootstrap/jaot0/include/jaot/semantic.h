#pragma once

#include "jaot/parser.h"

namespace JAOT {
    class SematicAnalyzer {
    public:
        void analyze(const Program &program) const;
    };
}