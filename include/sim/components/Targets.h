#ifndef TARGETS_H
#define TARGETS_H
#include "sim/Types.h"

namespace sim {
    // The actual move target
    struct Target {
        dim_t x = 0, y = 0;
    };

    // Static targets

    // Target randomly
    struct RandomTarget {};

    // Target static entity
    struct StaticEntityTarget {
        id_t target_entity = NO_ID;
    };

    // Avoid static entity
    struct StaticEntityAvoid {
        id_t target_entity = NO_ID;
    };

    // Dynamic targets

    // Target closest entity of type T
    template<typename T>
    struct FollowClosest {};

    // Avoid the closest entity of type T
    template<typename T>
    struct AvoidClosest {};
}

#endif //TARGETS_H
