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

    template<typename E>
    void operator()(E& e, const event::Cycle, Context& ctx) const {
        // auto e = simulation.template get_entities_of_type<EntityB>()[0];
        // ComponentB c = e.template get_component<ComponentB>();
        std::cout << "Cycle"  " from A!" << a << std::endl;
    }
};

struct TestEntity : Entity<TestEntity, TestComponent> {
    explicit TestEntity(int a): Entity(a) {
    }
};

class Fake {};

int main() {
    auto e = TestEntity(5);
    // auto e2 = EntityB();
    auto s = Simulation<TestEntity>();
    s.entities().add(e);
    // s.add(e2);
    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
