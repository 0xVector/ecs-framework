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
}

#endif //TRANSFORM_H
