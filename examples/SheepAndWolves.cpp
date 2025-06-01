#include <iostream>

#include "sim/Simulation.h"
#include "sim/components/Sprite.h"
#include "sim/components/Transform.h"
#include "sim/systems/Interactor.h"
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
    struct Sheep {};

    auto s = Simulation<Components<>, Systems<> >()
            .with_components<>()
            .with_systems<Movement, WorldBoundary,
                MoveToClosestResolver<Sheep>, TouchProcessor<Sheep>,
                Renderer>();

    Sprite sheep(Color{220, 210, 193});
    Sprite wolf{};

    constexpr int sheep_count = 100;
    constexpr int wolves = 10;

    constexpr uint SEED = 42;
    std::mt19937 rng{SEED};
    std::uniform_int_distribution rand_coord{0, 1000};
    std::uniform_int_distribution rand_speed{1, 10};

    for (int i = 0; i < sheep_count; ++i)
        s.create()
                .emplace<Sheep>()
                .emplace<Transform>(rand_coord(rng), rand_coord(rng)).emplace<Movable>(rand_speed(rng))
                .emplace<RandomTarget>()
                .emplace<Sprite>(sheep);

    for (int i = 0; i < wolves; ++i)
        s.create()
                .emplace<Transform>(rand_coord(rng), rand_coord(rng)).emplace<Movable>(rand_speed(rng))
                .emplace<EntityTarget>().emplace<MoveToClosest<Sheep> >()
                .emplace<TouchDestroys<Sheep> >()
                .push_back(wolf);

    s.run(10000);

    std::cout << "Done" << std::endl;
    return 0;
}
