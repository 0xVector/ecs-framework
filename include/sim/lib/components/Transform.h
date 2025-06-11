#ifndef TRANSFORM_H
#define TRANSFORM_H

/// @brief The namespace sim::lib contains the included components and systems library.
namespace sim::lib {
    /// @brief The spatial dimensions type used in Transform and related components.
    using dim_t = int; // Dimension type for coordinates and sizes

    /// @brief The Transform component holds the position of an entity in a 2D space.
    struct Transform {
        /// @brief The x-coordinate of the entity's position.
        dim_t x = 0;

        /// @brief The y-coordinate of the entity's position.
        dim_t y = 0;
    };

    /// @brief The Movable component indicates that an entity can move and defines its speed.
    struct Movable {
        /// @brief The speed of the entity.
        dim_t speed = 1;
    };
}

#endif //TRANSFORM_H
