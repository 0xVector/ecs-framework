#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <random>
#include <ranges>

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
            ctx.view<Transform, Movable, RandomTarget>().for_each([this](auto& t, auto& m, auto&) {
                t.x += m.speed * dist_(rng_);
                t.y += m.speed * dist_(rng_);
            });
        }
    };

    struct Movement {
        static constexpr uint SEED = 42; // Fixed seed for reproducibility

    private:
        std::mt19937 rng_{SEED};
        std::uniform_int_distribution<dim_t> dist_{-1, 1};

    public:
        void operator()(const event::Cycle, Context& ctx) {
            // Move entities with RandomPositionTarget
            ctx.view<Transform, Movable, RandomTarget>().for_each([this](auto& t, auto& m, auto&) {
                t.x += m.speed * dist_(rng_);
                t.y += m.speed * dist_(rng_);
            });

            // Move entities with PositionTarget
            ctx.view<Transform, Movable, PositionTarget>().for_each([](auto& t, auto& m, auto& p) {
                move_towards(t, Transform(p.x, p.y), m);
            });

            // Move entities with EntityTarget
            ctx.view<Transform, Movable, EntityTarget>().for_each([&](auto& t, auto& m, auto& target) {
                const Entity target_entity = ctx.get_entity(target.target_entity);
                if (target_entity.has<Transform>()) {
                    move_towards(t, target_entity.get<Transform>(), m);
                }
            });
        }

    private:
        static void move_towards(Transform& t, const Transform& target, const Movable& m) {
            const auto dx = target.x - t.x;
            const auto dy = target.y - t.y;

            // Move towards the target but clamp to not overshoot the target
            if (dx != 0) {
                t.x += m.speed * (dx > 0 ? 1 : -1);
                if ((dx > 0 && t.x > target.x) || (dx < 0 && t.x < target.x))
                    t.x = target.x; // Clamp to target
            }

            if (dy != 0) {
                t.y += m.speed * (dy > 0 ? 1 : -1);
                if ((dy > 0 && t.y > target.y) || (dy < 0 && t.y < target.y))
                    t.y = target.y; // Clamp to target
            }
        }
    };

    template<typename... ClosestWithTs>
    struct MoveToClosestResolver {
        void operator()(const event::PreCycle, Context& ctx) const {
            (resolve<ClosestWithTs>(ctx), ...);
        }

    private:
        template<typename T>
        static void resolve(Context& ctx) {
            ctx.view<Transform, EntityTarget, MoveToClosest<T> >()
                    .for_each([&](Entity& entity, Transform&, EntityTarget& target, MoveToClosest<T>&) {
                        auto dist = [&entity](Entity& to) {
                            const auto& [x1, y1] = entity.get<Transform>();
                            const auto& [x2, y2] = to.get<Transform>();
                            return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
                        };

                        auto potential_targets = ctx.view<Transform, T>();
                        if (potential_targets.empty()) {
                            target.target_entity = NO_ID; // No targets available
                            return;
                        }

                        const Entity closest = std::ranges::min(potential_targets, {}, dist);
                        target.target_entity = closest.id();
                    });
        }
    };
}

#endif //MOVEMENT_H
