#ifndef WORLD_H
#define WORLD_H
#include "sim/Event.h"
#include "sim/Types.h"
#include "sim/View.h"
#include "sim/components/Transform.h"

namespace sim {
    struct WorldBoundary {
        static constexpr dim_t MIN_X = 0;
        static constexpr dim_t MIN_Y = 0;
        static constexpr dim_t MAX_X = 1000;
        static constexpr dim_t MAX_Y = 1000;

        void operator()(const event::PostCycle, Context ctx) const {
            ctx.view<Transform>().for_each([](auto& t) {
                if (t.x < MIN_X) t.x = MIN_X;
                if (t.y < MIN_Y) t.y = MIN_Y;
                if (t.x > MAX_X) t.x = MAX_X;
                if (t.y > MAX_Y) t.y = MAX_Y;
            });
        }
    };
}

#endif //WORLD_H
