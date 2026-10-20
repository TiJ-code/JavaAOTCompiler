#pragma once

#include <unordered_map>

#include "jaot/ir.h"

namespace JAOT {
    class FrameLayout {
    public:
        void build(const IR::Method &method);

        int offset(IR::LocalId id) const;
        TypeRef type(IR::LocalId id) const;

        int frameSize() const;

    private:
        static int localSize(const TypeRef &type);

        std::unordered_map<IR::LocalId, int> offsets_;
        std::unordered_map<IR::LocalId, TypeRef> types_;
        int size_ = 0;
    };
}