#pragma once

#include <cstddef>

namespace JAOT {
    enum class Type {
        Int,
        Boolean,
        Reference,
        Void,
    };

    struct SourceLocation {
        std::size_t line = 0;
        std::size_t column = 0;
    };
}