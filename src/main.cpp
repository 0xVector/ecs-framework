#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestComponent {
    int a;

    explicit TestComponent(const int val):
        a(val) {
    }

    void operator()(const event::Cycle) const {
        std::cout << "Cycle!" << std::endl;
    }
};

struct TestEntity : Entity<TestEntity, TestComponent> {
    explicit TestEntity(int a): Entity(a) {
    }
};

int main() {
    auto e = TestEntity(5);
    auto s = Simulation<TestEntity>();
    s.add(e);
    s.run();

    std::cout << "Done" << std::endl;
    return 0;
}
