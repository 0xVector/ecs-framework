#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestComponentB;

struct TestComponentA {
    int a;

    explicit TestComponentA(const int val): a(val) {}

    void operator()(const event::Cycle) const {
        std::cout << "Simple cycle A (" << a << ")" << std::endl;
    }

    void operator()(const event::Cycle, Context& ctx) const {
        ctx.view<TestComponentA, TestComponentB>().for_each([this](const TestComponentA& a, const TestComponentB& b) {
            std::cout << "Complex cycle A (" << a.a << ", " << ")" << std::endl;
        });
        std::cout << "Simple cycle A (" << a << ")" << std::endl;
    }
};

struct TestComponentB {
    int a;

    explicit TestComponentB(const int val): a(val) {}

    void operator()(const event::Cycle) const {
        std::cout << "Simple cycle B (" << a << ")" << std::endl;
    }
};

struct TestEntity {};

using TestEntity_t = Entity<TestEntity, TestComponentA>;

int main() {
    Simulation<TestComponentA, TestComponentB> s;
    auto e1 = s.create();
    auto e2 = s.create();

    e1.emplace<TestComponentA>(5);

    e2.emplace<TestComponentA>(1);
    e2.emplace<TestComponentB>(2);

    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
