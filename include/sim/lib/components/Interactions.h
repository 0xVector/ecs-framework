#ifndef INTERACTIONS_H
#define INTERACTIONS_H
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    template<typename Touchable>
    struct DestroyByTouch {
        dim_t min_distance = 5;
    };
};

#endif //INTERACTIONS_H
