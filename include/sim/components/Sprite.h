#ifndef SPRITE_H
#define SPRITE_H

namespace sim {
    struct Color {
        using color_t = unsigned char;
        color_t r, g, b, a;
    };

    struct Sprite {
        Color color;
    };
}

#endif //SPRITE_H
