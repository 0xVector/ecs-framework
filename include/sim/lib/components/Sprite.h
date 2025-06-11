#ifndef SPRITE_H
#define SPRITE_H
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    struct Color {
        using color_t = unsigned char; // Color component type (RGBA)
        color_t r = 0, g = 0, b = 0, a = 255;
    };

    struct Sprite {
        static constexpr dim_t DEFAULT_SIZE = 10; // Default size for width and height

        Color color;
        dim_t width = DEFAULT_SIZE;
        dim_t height = DEFAULT_SIZE;
    };
}

#endif //SPRITE_H
