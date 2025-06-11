#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace sim::lib {
    using dim_t = int; // Dimension type for coordinates and sizes

    struct Transform {
        dim_t x = 0, y = 0;
    };

    struct Movable {
        dim_t speed = 1;
    };
}

#endif //TRANSFORM_H
