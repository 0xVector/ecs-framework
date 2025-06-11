#include <iostream>

#include "sim/Simulation.h"

using namespace sim;

struct Component1 {
    int a;
};

struct Component2 {
    int b;
};

struct SystemA {
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
        ctx.view<Component1>().for_each([&](const Entity entity, const Component1& a) {
            std::cout << "[A]: Cycle " << ctx.cycle() << " "
                    "for entity #" << entity.id() << " "
                    "with TestComponent1(" << a.a << ")" << std::endl;
        });
    }
};

struct SystemB {
    void operator()(const event::Cycle, Context ctx) const {
        for (Entity entity: ctx.view<Component1, Component2>()) {
            std::cout << "[B]: Cycle " << ctx.cycle() << " "
                    "for entity #" << entity.id() << " "
                    "with TestComponent1(" << entity.get<Component1>().a << ") "
                    "and TestComponent2(" << entity.get<Component2>().b << ")" << std::endl;
        }
    }
};

int main() {
    auto s = Simulation<SystemA>()
            .with_systems<SystemB>();

    s.create()
            .emplace<Component1>(1);

    s.create()
            .emplace<Component2>(2);

    s.create()
            .emplace<Component1>(10)
            .emplace<Component2>(20);

    s.run(3);
    return 0;
}
