#ifndef TRANSFORMUTILS_H
#define TRANSFORMUTILS_H

#include "sim/lib/components/Transform.h"

namespace sim::lib {
    inline dim_t dist_squared(const Transform& a, const Transform& b) {
        const dim_t dx = a.x - b.x;
        const dim_t dy = a.y - b.y;
        return dx * dx + dy * dy;
    }
} // namespace sim

#endif //TRANSFORMUTILS_H
