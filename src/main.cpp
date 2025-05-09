#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestComponent;

struct TestEntity {};
using TestEntity_t = Entity<TestEntity, TestComponent>;

struct TestComponent {
    int a;

    explicit TestComponent(const int val):
        a(val) {
    }

    template<typename E>
    void operator()(E& e, const event::Cycle, auto& ctx) const {
        std::cout << "Cycle"  " from A!" << a << std::endl;
        auto entities = ctx.template get_entity<TestEntity_t>();
    }
};

int main() {
    auto e = TestEntity_t();
    auto s = Simulation<TestEntity_t>();
    e.spawn(5);
    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
