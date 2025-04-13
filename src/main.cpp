#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestEntity;

struct EntityB;

struct ComponentB {
    std::string name = "BBB";

    template<typename... E>
    void operator()(const event::Cycle, Simulation<E...>& sim) const {
        std::cout << "Im " << name <<  " at " << sim.cycle() << std::endl;
    }
};

struct TestComponent {
    int a;

    explicit TestComponent(const int val):
        a(val) {
    }

    template<typename... Entities>
    void operator()(const event::Cycle, Simulation<Entities...>& simulation) const {
        auto e = simulation.template get_entities_of_type<EntityB>()[0];
        ComponentB c = e.template get_component<ComponentB>();
        std::cout << "Cycle from A!" << a << " with friend: " << c.name << std::endl;
    }
};

struct TestEntity : Entity<TestEntity, TestComponent> {
    explicit TestEntity(int a): Entity(a) {
    }
};

struct EntityB : Entity<EntityB, ComponentB> {
    explicit EntityB() : Entity() {

    }
};

class Fake {};

int main() {
    auto e = TestEntity(5);
    auto e2 = EntityB();
    auto s = Simulation<TestEntity, EntityB>();
    s.add(e);
    s.add(e2);
    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
