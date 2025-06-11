#ifndef WORLD_H
#define WORLD_H
#include "sim/Event.h"
#include "sim/View.h"
#include "sim/lib/components/Transform.h"

namespace sim::lib {
    /// @brief A system that enforces world boundaries for entities with Transform components.
    struct WorldBoundary {
        static constexpr dim_t MIN_X = 0; // Minimum X coordinate
        static constexpr dim_t MIN_Y = 0; // Minimum Y coordinate
        static constexpr dim_t MAX_X = 1000; // Maximum X coordinate
        static constexpr dim_t MAX_Y = 1000; // Maximum Y coordinate

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
