#ifndef SPRITE_H
#define SPRITE_H

namespace sim {
    struct Color {
        using color_t = unsigned char;
        color_t r, g, b, a;
    };

    struct Sprite {
        using dim_t = int;
        static constexpr Color DEFAULT_COLOR{0, 0, 0, 255};
        static constexpr dim_t DEFAULT_SIZE = 10;

        Color color = DEFAULT_COLOR;
        dim_t width = DEFAULT_SIZE;
        dim_t height = DEFAULT_SIZE;
    };
}

#endif //SPRITE_H
