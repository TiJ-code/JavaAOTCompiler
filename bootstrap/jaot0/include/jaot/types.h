#pragma once

#include <cstddef>
#include <string>

namespace JAOT {
    enum class Type {
        Int,
        Boolean,
        Reference,
        Void,
    };

    struct TypeRef {
        Type kind;
        std::string className;
    };

    struct SourceLocation {
        std::size_t line = 0;
        std::size_t column = 0;
    };
}
