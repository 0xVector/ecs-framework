#include <iostream>

#include "sim/Simulation.h"
#include "sim/lib/components/Sprite.h"
#include "sim/lib/components/Transform.h"
#include "sim/lib/systems/Interactor.h"
#include "sim/lib/systems/Movement.h"
#include "sim/lib/systems/Renderer.h"
#include "sim/lib/systems/World.h"

using namespace sim;
using namespace sim::lib;

struct TestSystem {
    void operator()(const event::Cycle, Context ctx) const {
        ctx.view<Transform, Movable>().for_each([](const Entity e, auto& t, auto& m) {
            std::cout << "Entity #" << e.id() << " at (" << t.x << ", " << t.y << ") with speed " << m.speed <<
                    std::endl;
        });
    }
};

int main() {
    auto s = Simulation<TestSystem>()
            .with_systems<Movement, TargetResolver<RandomTarget> >()
            .with_systems<WorldBoundary, Renderer>();

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
