#pragma once

#include "jaot/frame.h"
#include "jaot/ir.h"
#include "jaot/machine.h"
#include "jaot/runtime_codegen.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

namespace JAOT {
    class ExpressionLowerer {
    public:
        ExpressionLowerer(
            Machine &machine,
            RuntimeEmitter &runtime,
            const FrameLayout &frame,
            const std::unordered_map<std::string, MethodSignature> &methods,
            const std::unordered_map<std::string, IR::Class> &classes,
            std::string methodName
        );

        ValueLocation lower(const IR::Expr &expression);

    private:
        [[nodiscard]] ValueKind valueKind(const TypeRef &type) const;

        ValueLocation lowerFieldAccess(const IR::Expr &expression);
        ValueLocation lowerUnary(const IR::Expr &expression);
        ValueLocation lowerBinary(const IR::Expr &expression);
        ValueLocation lowerCall(const IR::Expr &expression);

        std::string makeLabel(std::string_view prefix);

        Machine &machine_;
        RuntimeEmitter &runtime_;
        const FrameLayout &frame_;
        const std::unordered_map<std::string, MethodSignature> &methods_;
        const std::unordered_map<std::string, IR::Class> &classes_;
        std::string methodName_;
        std::size_t labelCounter_ = 0;
    };
}