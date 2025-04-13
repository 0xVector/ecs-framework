#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestEntity;

struct TestComponent {
    int a;

    explicit TestComponent(const int val):
        a(val) {
    }

    void operator()(const event::Cycle) const {
        std::cout << "Cycle!" << a << " (no state)" << std::endl;
    }

    template<typename... Entities>
    void operator()(const event::Cycle, Simulation<Entities...>& simulation) const {
        auto e = simulation.template get_entities_of_type<TestEntity>();
        std::cout << "Cycle!" << a << " " << std::get<0>(e[0].components).a << std::endl;
    }
};

struct TestEntity : Entity<TestEntity, TestComponent> {
    explicit TestEntity(int a): Entity(a) {
    }
};

class Fake {};

int main() {
    auto e = TestEntity(5);
    auto s = Simulation<TestEntity>();
    s.add(e);
    s.run();

    std::cout << "Done" << std::endl;
    return 0;
}
