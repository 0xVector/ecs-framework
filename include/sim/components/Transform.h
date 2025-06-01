#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "sim/Types.h"

namespace sim {
    struct Transform {
        dim_t x = 0, y = 0;
    };

    struct Movable {
        dim_t speed = 1;
    };

    struct PositionTarget {
        dim_t x = 0, y = 0;
    };

    struct EntityTarget {
        id_t target_entity;
    };

    struct RandomTarget {};

    template<typename T>
    struct FollowClosest {};
}

#endif //TRANSFORM_H
