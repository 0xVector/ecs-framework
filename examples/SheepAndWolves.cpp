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
    void operator()(const event::Cycle, Context ctx) const {
        ctx.view<Transform, Movable>().for_each([](const Entity e, auto& t, auto& m) {
            std::cout << "Entity #" << e.id() << " at (" << t.x << ", " << t.y << ") with speed " << m.speed <<
                    std::endl;
        });
    }
};

int main() {
    struct Grass {};
    struct Sheep {};
    struct Wolf {};

    auto s = Simulation<
        Movement, WorldBoundary,
        TargetResolver<Sheep, Grass, Wolf>,
        TouchableTargets<Sheep, Grass>,
        Renderer
    >();


    Sprite grass(Color{60, 240, 80});
    Sprite sheep(Color{220, 210, 193});
    Sprite wolf{};

    constexpr int grass_count = 2000;
    constexpr int sheep_count = 100;
    constexpr int wolves = 5;

    constexpr uint SEED = 42;
    std::mt19937 rng{SEED};
    std::uniform_int_distribution rand_coord{0, 1000};
    std::uniform_int_distribution rand_speed{1, 3};

    // Grass
    for (int i = 0; i < grass_count; ++i)
        s.create()
                .emplace<Grass>()
                .emplace<Transform>(rand_coord(rng), rand_coord(rng))
                .emplace<Sprite>(grass);

    // Sheep
    for (int i = 0; i < sheep_count; ++i)
        s.create()
                .emplace<Sheep>()
                .emplace<Transform>(rand_coord(rng), rand_coord(rng)).emplace<Movable>(rand_speed(rng))
                .emplace<Target>()
                .emplace<FollowClosest<Grass> >()
                .emplace<AvoidClosest<Wolf> >()
                .emplace<DestroyByTouch<Grass> >()
                .emplace<Sprite>(sheep);

    // Wolves
    for (int i = 0; i < wolves; ++i)
        s.create()
                .emplace<Wolf>()
                .emplace<Transform>(rand_coord(rng), rand_coord(rng)).emplace<Movable>(rand_speed(rng))
                .emplace<Target>()
                .emplace<FollowClosest<Sheep> >()
                .emplace<DestroyByTouch<Sheep> >()
                .push_back(wolf);

    s.run(10000);

    std::cout << "Done" << std::endl;
    return 0;
}
