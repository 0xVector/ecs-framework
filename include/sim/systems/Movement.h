#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <random>

#include "sim/Event.h"
#include "sim/View.h"
#include "sim/components/Transform.h"
#include "sim/Types.h"

namespace sim {
    struct RandomMovement {
        static constexpr uint SEED = 42; // Fixed seed for reproducibility

    private:
        std::mt19937 rng_{SEED};
        std::uniform_int_distribution<dim_t> dist_{-1, 1};

    public:
        void operator()(const event::Cycle, Context& ctx) {
            ctx.view<Transform, Movable, RandomPositionTarget>().for_each([this](auto& t, auto& m, auto&) {
                t.x += m.speed * dist_(rng_);
                t.y += m.speed * dist_(rng_);
            });
        }
    };

    template<typename... ClosestWithTs>
    struct MoveToClosestResolver {
        void operator()(const event::PreCycle, Context& ctx) {
            (resolve<ClosestWithTs>(ctx), ...);
        }

    private:
        template<typename T>
        void resolve(Context& ctx) {
            ctx.view<MoveToClosestWith<T>>().for_each([&](auto& m) {
            });
        }
    };
}

#endif //MOVEMENT_H
