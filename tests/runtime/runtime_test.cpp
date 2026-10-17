#include "runtime.h"
#include "../../runtime/runtime.h"

#include <cstdint>
#include <cstdlib>

struct ObjectLayout {
    std::int32_t value;
    std::uint32_t enabled;
    void *next;
};

int main() {
    auto *first = static_cast<ObjectLayout *>(jaot_allocate(sizeof(ObjectLayout)));
    auto *second = static_cast<ObjectLayout *>(jaot_allocate(sizeof(ObjectLayout)));

    const bool fieldsAreZero =
        first->value == 0 &&
        first->enabled == 0 &&
        first->next == nullptr;

    const bool objectsAreDistinct = first != second;

    void *emptyFirst = jaot_allocate(0);
    void *emptySecond = jaot_allocate(0);

    const bool emptyObjectsAreDistinct = emptyFirst != emptySecond;

    std::free(first);
    std::free(second);
    std::free(emptyFirst);
    std::free(emptySecond);

    return fieldsAreZero && objectsAreDistinct && emptyObjectsAreDistinct
        ? 0
        : 1;
}