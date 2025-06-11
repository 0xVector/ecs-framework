#ifndef INTERACTIONS_H
#define INTERACTIONS_H
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    /// @brief A component that signals that the entity can destroy another entity by touching it.
    /// @tparam Touchable The type of component that the target entity must have. Only entities with this component will be checked.
    template<typename Touchable>
    struct DestroyByTouch {
        /// @brief The minimal distance at which the entity can destroy another entity.
        dim_t min_distance = 5;
    };
};

#endif //INTERACTIONS_H
