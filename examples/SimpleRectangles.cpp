#include <iostream>

#include "sim/Simulation.h"
#include "sim/components/Sprite.h"
#include "sim/components/Transform.h"
#include "sim/systems/Movement.h"
#include "sim/systems/Renderer.h"
#include "sim/systems/World.h"

using namespace sim;

struct TestSystem {
    void operator()(const event::Cycle, Context& ctx) const {
        ctx.view<Transform, Movable>().for_each([](const Entity e, auto& t, auto& m) {
            std::cout << "Entity #" << e.id() << " at (" << t.x << ", " << t.y << ") with speed " << m.speed <<
                    std::endl;
        });
    }
};

int main() {
    auto s = Simulation<Components<>, Systems<TestSystem> >()
            .with_components<>()
            .with_systems<Movement, TargetResolver<RandomTarget>, WorldBoundary, Renderer>();

    Sprite red(Color{255, 0, 0, 255});
    Sprite green(Color{0, 255, 0, 255});

    const auto top_left = s.create().emplace<Transform>(0, 0);
    const auto bottom_right = s.create().emplace<Transform>(1000, 1000);

    s.create()
            .emplace<Transform>(500, 500).emplace<Movable>(10)
            .emplace<Target>().emplace<RandomTarget>()
            .emplace<Sprite>(green);

    s.create()
            .emplace<Transform>(500, 500).emplace<Movable>(5)
            .emplace<Target>().emplace<StaticEntityTarget>(bottom_right.id())
            .emplace<Sprite>(green);

    s.create()
            .emplace<Transform>(500, 500).emplace<Movable>(5)
            .emplace<Target>().emplace<StaticEntityTarget>(top_left.id())
            .emplace<Sprite>(green);

    s.create()
            .emplace<Transform>(500, 600).emplace<Movable>()
            .emplace<Target>().emplace<FollowClosest<RandomTarget> >()
            .push_back(red);

    s.run(10000);

    std::cout << "Done" << std::endl;
    return 0;
}
