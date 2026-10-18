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

    inline bool sameType(const TypeRef &left, const TypeRef &right) {
        return left.kind == right.kind &&
            (left.kind != Type::Reference ||
             left.className == right.className);
    }

    inline bool isAssignable(const TypeRef &source, const TypeRef &target) {
        if (source.kind != target.kind) {
            return false;
        }

        if (source.kind != Type::Reference) {
            return true;
        }

        return source.className.empty() || source.className == target.className;
    }

    inline std::string typeName(const TypeRef &type) {
        switch (type.kind) {
            case Type::Int:
                return "int";
            case Type::Boolean:
                return "boolean";
            case Type::Void:
                return "void";
            case Type::Reference:
                return type.className.empty() ? "null" : type.className;
        }

        return "<unknown type>";
    }

    struct SourceLocation {
        std::size_t line = 0;
        std::size_t column = 0;
    };
}
