#include <iostream>

#include "sim/Simulation.h"

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

int main() {
    auto s = Simulation<Components<>, Systems<> >()
            .with_components<TestComponentA, TestComponentB>()
            .with_systems<TestSystemA, TestSystemB>();
    auto e1 = s.create();
    auto e2 = s.create();

    e1.emplace<TestComponentA>(5);

    e2.emplace<TestComponentA>(1);
    e2.emplace<TestComponentB>(2);

    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
