#include <iostream>

#include "sim/Simulation.h"
#include "sim/components/Sprite.h"
#include "sim/components/Transform.h"
#include "sim/systems/Movement.h"
#include "sim/systems/Renderer.h"

using namespace sim;

struct TestComponent1 {
    int a;
};

struct TestComponent2 {
    int b;
};

struct TestSystemA {
    void operator()(const event::SimStart) const {
        std::cout << "Simple start A" << std::endl;
    }

    void operator()(const event::Cycle) const {
        std::cout << "Simple cycle A" << std::endl;
    }
};

struct TestSystemB {
    void operator()(const event::Cycle) const {
        std::cout << "Simple cycle B" << std::endl;
    }

    void operator()(const event::Cycle, Context& ctx) const {
        ctx.view<TestComponent1, TestComponent2>().for_each(
            [](const TestComponent1& a, const TestComponent2& b) {
                std::cout << "Complex cycle B (" << a.a << ", " << b.b << ")" << std::endl;
            });
    }
};

int main() {
    auto s = Simulation<Components<>, Systems<> >()
            .with_components<TestComponent1, TestComponent2>()
            .with_systems<TestSystemA, TestSystemB, RandomMovement, Renderer>();

    Sprite red(Color{255, 0, 0, 255});
    Sprite green(Color{0, 255, 0, 255});

    s.create()
            .emplace<TestComponent1>(5)
            .emplace<Transform>(500, 500).emplace<Movable>()
            .emplace<RandomPositionTarget>()
            .emplace<Sprite>(red);

    s.create()
            .emplace<TestComponent1>(1)
            .emplace<TestComponent2>(2)
            .emplace<Transform>(500, 600).emplace<Movable>()
            .emplace<RandomPositionTarget>()
            .push_back(green);

    s.run(100);

    std::cout << "Done" << std::endl;
    return 0;
}
