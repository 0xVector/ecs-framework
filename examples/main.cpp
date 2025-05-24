#include <iostream>

#include "sim/Simulation.h"
#include "sim/components/Sprite.h"
#include "sim/components/Transform.h"
#include "sim/systems/Renderer.h"

using namespace sim;

struct TestComponentA {
    int a;
};

struct TestComponentB {
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
        ctx.view<TestComponentA, TestComponentB>().for_each(
            [](const TestComponentA& a, const TestComponentB& b) {
                std::cout << "Complex cycle B (" << a.a << ", " << b.b << ")" << std::endl;
            });
    }
};

struct Movement {
    void operator()(const event::Cycle, Context& ctx) const {
        ctx.view<sim::Transform>().for_each([](auto& t) {
            t.x += 1;
            t.y += 1;
        });
    }
};

int main() {
    auto s = Simulation<Components<>, Systems<> >()
            .with_components<TestComponentA, TestComponentB>()
            .with_systems<TestSystemA, TestSystemB, Movement, Renderer>();
    auto e1 = s.create();
    auto e2 = s.create();

    Sprite red(Color{255, 0, 0, 255});
    Sprite green(Color{0, 255, 0, 255});

    e1.emplace<TestComponentA>(5);
    e1.emplace<Transform>(0, 0);
    e1.emplace<Sprite>(red);

    e2.emplace<TestComponentA>(1);
    e2.emplace<TestComponentB>(2);
    e2.emplace<Transform>(50, 100);
    e2.push_back(green);

    s.run(100);

    std::cout << "Done" << std::endl;
    return 0;
}
