#pragma once

#include "jaot/parser.h"

namespace JAOT {
    class SematicAnalyzer {
    public:
        static void analyze(const Program &program) ;

    private:
        struct AnalysisContext;

        static void collectClassNames(
            const Program &program,
            AnalysisContext &context
        );

        static void validateMethodTypes(
            const Program &program,
            const AnalysisContext &context
        );

        static void validateRequiredMainClass(
            const Program &program
        );

        static void collectFields(
            const Program &program,
            AnalysisContext &context
        );

        static void collectMethods(
            const Program &program,
            AnalysisContext &context
        );

        static void validateEntryPoint(
            const AnalysisContext &context
        );

        static void analyzeMethodBodies(
            const Program &program,
            const AnalysisContext &context
        );
    };
}