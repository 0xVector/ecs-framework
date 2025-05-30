#ifndef SPRITE_H
#define SPRITE_H
#include "sim/Types.h"

namespace sim {
    struct Color {
        using color_t = unsigned char; // Color component type (RGBA)
        color_t r, g, b, a;
    };

    struct Sprite {
        static constexpr Color DEFAULT_COLOR{0, 0, 0, 255}; // Default color
        static constexpr dim_t DEFAULT_SIZE = 10; // Default size for width and height

        Color color = DEFAULT_COLOR;
        dim_t width = DEFAULT_SIZE;
        dim_t height = DEFAULT_SIZE;
    };
}

#endif //SPRITE_H
