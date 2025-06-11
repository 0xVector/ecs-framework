#ifndef TARGETS_H
#define TARGETS_H
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    /// @brief Represents a movement target.
    struct Target {
        /// @brief The target x, y coordinates in the world.
        dim_t x = 0, y = 0;
    };

    // Static targets

    /// @brief A random target provider
    struct RandomTarget {};

    /// @brief A static target provider that targets a specific entity.
    struct StaticEntityTarget {
        /// @brief The ID of the target entity.
        id_t target_entity = NO_ID;
    };

    /// @brief A static target provider that avoids a specific entity.
    /// This is useful for entities that should not collide with or approach a specific entity.
    struct StaticEntityAvoid {
        /// @brief The ID of the entity to avoid.
        id_t target_entity = NO_ID;
    };

    // Dynamic targets

    /// @brief A dynamic target provider that targets the closest entity with a component of type T.
    /// This is useful for entities that should follow or interact with the nearest entity having a mark or a specific component.
    /// @tparam T The type of component that the target entity must have.
    template<typename T>
    struct FollowClosest {};

    // Avoid the closest entity of type T
    /// @brief A dynamic target provider that avoids the closest entity of type T.
    /// This is useful for entities that should avoid the nearest entity having a mark or a specific component.
    /// @tparam T The type of component that the target entity must have.
    template<typename T>
    struct AvoidClosest {};
}

#endif //TARGETS_H
