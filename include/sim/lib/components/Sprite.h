#ifndef SPRITE_H
#define SPRITE_H
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    /// @brief Represents a color with RGBA components.
    struct Color {
        /// @brief Type alias for color components.
        using color_t = unsigned char; // Color component type (RGBA)

        /// @brief The RGBA components of the color.
        color_t r = 0, g = 0, b = 0, a = 255;
    };

    /// @brief Represents a sprite with a color and dimensions.
    struct Sprite {
        /// @brief Default size for the sprite's width and height.
        static constexpr dim_t DEFAULT_SIZE = 10; // Default size for width and height

        /// @brief The color of the sprite.
        Color color;

        /// @brief The width of the sprite.
        dim_t width = DEFAULT_SIZE;

        /// @brief The height of the sprite.
        dim_t height = DEFAULT_SIZE;
    };
}

#endif //SPRITE_H
