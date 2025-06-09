#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <random>
#include <ranges>

#include "sim/Event.h"
#include "sim/View.h"
#include "sim/components/Transform.h"
#include "sim/Types.h"
#include "sim/components/Targets.h"

namespace sim {
    struct Movement {
        void operator()(const event::Cycle, Context& ctx) const {
            ctx.view<Transform, Movable, Target>()
                    .for_each([&](Transform& t, const Movable& m, const Target& to) {
                        const auto dx = to.x - t.x;
                        const auto dy = to.y - t.y;

                        // Move towards the target but clamp to not overshoot the target
                        if (dx != 0) {
                            t.x += m.speed * (dx > 0 ? 1 : -1);
                            if ((dx > 0 && t.x > to.x) || (dx < 0 && t.x < to.x))
                                t.x = to.x; // Clamp to target
                        }

                        if (dy != 0) {
                            t.y += m.speed * (dy > 0 ? 1 : -1);
                            if ((dy > 0 && t.y > to.y) || (dy < 0 && t.y < to.y))
                                t.y = to.y; // Clamp to target
                        }
                    });
        }
    };

    template<typename... DynamicTs>
    struct TargetResolver {
        static constexpr uint SEED = 42; // Fixed seed for reproducibility
        static constexpr dim_t RANDOM_MOVE_RANGE = 10; // Range for random movement

    private:
        std::mt19937 rng_{SEED};
        std::uniform_int_distribution<dim_t> dist_{-1, 1};

    public:
        void operator()(const event::SimStart, Context& ctx) const {}

        void operator()(const event::PreCycle, Context& ctx) {
            resolve_random(ctx);
            resolve_avoid_entity(ctx);
            resolve_target_entity(ctx);

            // Resolve dynamic targets
            (resolve_avoid_dynamic<DynamicTs>(ctx), ...);
            (resolve_follow_dynamic<DynamicTs>(ctx), ...);
        }

    private:
        void resolve_random(Context& ctx) {
            ctx.view<Transform, Target, RandomTarget>()
                    .for_each([this](const Transform& t, Target& to, RandomTarget&) {
                        to.x = t.x + dist_(rng_) * RANDOM_MOVE_RANGE;
                        to.y = t.y + dist_(rng_) * RANDOM_MOVE_RANGE;
                    });
        }

        static void resolve_target_entity(Context& ctx) {
            ctx.view<Transform, Target, StaticEntityTarget>()
                    .for_each([&](const Transform&, Target& to, const StaticEntityTarget& target) {
                        const auto target_entity = ctx.get_entity(target.target_entity);
                        if (!target_entity.has<Transform>()) return;

                        const auto [x, y] = target_entity.get<Transform>();
                        to.x = x;
                        to.y = y;
                    });
        }

        static void resolve_avoid_entity(Context& ctx) {
            ctx.view<Transform, Target, StaticEntityAvoid>()
                    .for_each([&](const Transform& t, Target& to, const StaticEntityAvoid& target) {
                        const auto target_entity = ctx.get_entity(target.target_entity);
                        if (!target_entity.has<Transform>()) return;

                        const auto [x, y] = target_entity.get<Transform>();
                        // Move away from the target
                        to.x = (x < t.x) ? 1 : -1; // Move away in x direction
                        to.y = (y < t.y) ? 1 : -1; // Move away in y direction
                    });
        }

        template<typename T>
        static void resolve_follow_dynamic(Context& ctx) {
            ctx.view<Transform, Target, FollowClosest<T> >()
                    .for_each([&](const Entity& self, Transform& t, Target& to, FollowClosest<T>&) {
                        // Dist metric
                        auto dist = [&](const Entity& to_entity) {
                            if (self.id() == to_entity.id())
                                return std::numeric_limits<dim_t>::max(); // Ignore self

                            const auto& [s_x, s_y] = t;
                            const auto& [to_x, to_y] = to_entity.get<Transform>();
                            return (s_x - to_x) * (s_x - to_x) + (s_y - to_y) * (s_y - to_y);
                        };

                        auto potential_targets = ctx.view<Transform, T>();
                        if (potential_targets.empty()) {
                            return;
                            to.x = t.x; // No targets to follow, stay in place
                            to.y = t.y;
                        }

                        const Entity closest = std::ranges::min(potential_targets, {}, dist);
                        const auto [x, y] = closest.get<Transform>();
                        to.x = x;
                        to.y = y;
                    });
        }

        template<typename T>
        static void resolve_avoid_dynamic(Context& ctx) {
            ctx.view<Transform, Target, AvoidClosest<T> >()
                    .for_each([&](const Entity& self, Transform& t, Target& to, AvoidClosest<T>&) {
                        // Dist metric
                        auto dist = [&](const Entity& to_entity) {
                            if (self.id() == to_entity.id())
                                return std::numeric_limits<dim_t>::max(); // Ignore self

                            const auto& [s_x, s_y] = t;
                            const auto& [to_x, to_y] = to_entity.get<Transform>();
                            return (s_x - to_x) * (s_x - to_x) + (s_y - to_y) * (s_y - to_y);
                        };

                        auto potential_targets = ctx.view<Transform, T>();
                        if (potential_targets.empty()) {
                            to.x = t.x; // No targets to follow, stay in place
                            to.y = t.y;
                            return;
                        }

                        const Entity closest = std::ranges::min(potential_targets, {}, dist);
                        const auto [x, y] = closest.get<Transform>();
                        // Move away from the target
                        to.x = x < t.x ? t.x + 1 : t.x - 1; // Move away in x direction
                        to.y = y < t.y ? t.y + 1 : t.y - 1; // Move away in y direction
                    });
        }
    };
}

#endif //MOVEMENT_H
