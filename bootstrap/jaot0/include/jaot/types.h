#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace JAOT {
    enum class Type {
        Char,
        Byte,
        Short,
        Int,
        Long,
        Boolean,
        Reference,
        Array,
        Void,
    };

    struct TypeRef {
        Type kind;
        std::string className;
        std::shared_ptr<TypeRef> elementType;
    };

    struct MethodSignature {
        std::string name;
        std::string ownerClass;
        TypeRef returnType;
        std::vector<TypeRef> parameterTypes;
    };

    inline std::string methodSymbol(const MethodSignature &signature) {
        if (signature.ownerClass == "Main" && signature.name == "main") {
            return "main";
        }
        return signature.ownerClass + "_" + signature.name;
    }

    inline bool isReferenceType(const TypeRef &type) {
        return type.kind == Type::Reference || type.kind == Type::Array;
    }

    inline bool sameType(const TypeRef &left, const TypeRef &right) {
        if (left.kind != right.kind) {
            return false;
        }

        if (left.kind == Type::Reference) {
            return left.className == right.className;
        }

        if (left.kind == Type::Array) {
            return left.elementType &&
                   right.elementType &&
                   sameType(*left.elementType, *right.elementType);
        }

        return true;
    }

    inline bool isAssignable(const TypeRef &source, const TypeRef &target) {
        if (source.kind == Type::Reference &&
            source.className.empty() &&
            isReferenceType(target)) {
            return true;
        }

        if (source.kind == Type::Reference &&
            target.kind == Type::Reference) {
            return source.className == target.className;
        }

        if (source.kind == Type::Array &&
            target.kind == Type::Array) {
            return source.elementType &&
                   target.elementType &&
                   isAssignable(*source.elementType, *target.elementType);
        }

        return source.kind == target.kind;
    }

    inline std::string typeName(const TypeRef &type) {
        switch (type.kind) {
            case Type::Char:
                return "char";

            case Type::Byte:
                return "byte";

            case Type::Int:
                return "int";

            case Type::Short:
                return "short";

            case Type::Long:
                return "long";

            case Type::Boolean:
                return "boolean";

            case Type::Void:
                return "void";

            case Type::Reference:
                return type.className.empty() ? "null" : type.className;

            case Type::Array:
                return type.elementType
                ? typeName(*type.elementType) + "[]"
                : "<invalid array type>";
        }

        return "<unknown type>";
    }

    struct SourceLocation {
        std::size_t line = 0;
        std::size_t column = 0;
    };
}
