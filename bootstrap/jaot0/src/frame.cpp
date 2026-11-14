#include "jaot/frame.h"

namespace JAOT {
    int FrameLayout::localSize(const TypeRef &type) {
        if (type.kind == Type::Int) {
            return 4;
        }

        if (type.kind == Type::Short) {
            return 2;
        }
        if (type.kind == Type::Char) {
            return 1;
        }
        if (type.kind == Type::Byte) {
            return 1;
        }
        return type.kind == Type::Reference || type.kind == Type::Long ? 8 : 4;
    }

    void FrameLayout::build(const IR::Method &method) {
        offsets_.clear();
        types_.clear();
        size_ = 0;

        for (const IR::Local &local : method.locals) {
            const int size = localSize(local.type);
            const int offset = -size_ - size;

            if (!offsets_.emplace(local.id, offset).second) {
                throw std::runtime_error("duplicate IR local id");
            }

            types_.emplace(local.id, local.type);
            size_ += size;
        }

        for (const IR::LocalId parameter : method.parameters) {
            if (!offsets_.contains(parameter)) {
                throw std::runtime_error("method parameter has no IR local slot");
            }
        }
    }

    int FrameLayout::offset(const IR::LocalId id) const {
        const auto found = offsets_.find(id);

        if (found == offsets_.end()) {
            throw std::runtime_error("unknown IR local id");
        }

        return found->second;
    }

    TypeRef FrameLayout::type(const IR::LocalId id) const {
        const auto found = types_.find(id);

        if (found == types_.end()) {
            throw std::runtime_error("unknown IR local id");
        }

        return found->second;
    }

    int FrameLayout::frameSize() const {
        return (size_ + 15) & ~15;
    }
}
