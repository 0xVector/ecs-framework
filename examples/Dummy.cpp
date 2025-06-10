#include <iostream>

#include "sim/Simulation.h"

using namespace sim;

struct TestComponent1 {
    int a;
};

struct TestComponent2 {
    int b;
};

struct TestSystemA {
    void operator()(const event::SimStart) const {
        std::cout << "System A starting" << std::endl;
    }

    void operator()(const event::SimEnd) const {
        std::cout << "System A ending" << std::endl;
    }

    void operator()(const event::Cycle) const {
        std::cout << "[A]: Simple cycle" << std::endl;
    }

    void operator()(const event::Cycle, Context ctx) const {
        ctx.view<TestComponent1>().for_each([&](const Entity entity, const TestComponent1& a) {
            std::cout << "[A]: Cycle " << ctx.cycle() << " "
                    "for entity #" << entity.id() << " "
                    "with TestComponent1(" << a.a << ")" << std::endl;
        });
    }
};

struct TestSystemB {
    void operator()(const event::Cycle, Context ctx) const {
        for (Entity entity: ctx.view<TestComponent1, TestComponent2>()) {
            std::cout << "[B]: Cycle " << ctx.cycle() << " "
                    "for entity #" << entity.id() << " "
                    "with TestComponent1(" << entity.get<TestComponent1>().a << ") "
                    "and TestComponent2(" << entity.get<TestComponent2>().b << ")" << std::endl;
        }
    }
};

int main() {
    auto s = Simulation<Components<TestComponent1>, Systems<> >()
            .with_components<TestComponent2>()
            .with_systems<TestSystemA, TestSystemB>();

    s.create()
            .emplace<TestComponent1>(1);

    s.create()
            .emplace<TestComponent2>(2);

    s.create()
            .emplace<TestComponent1>(10)
            .emplace<TestComponent2>(20);

    s.run(3);
    return 0;
}
