#include <iostream>

#include "sim/Entity.h"
#include "sim/Event.h"
#include "sim/Simulation.h"

using namespace sim;

struct TestComponentA {
    int a;

    explicit TestComponentA(const int val): a(val) {}

    template<typename E, typename S>
    void operator()(E& e, const event::Cycle, Context<S>& ctx) const {
        ctx.template get_entity<E>();
        std::cout << "Cycle" " from A! " << a << std::endl;
    }
};

struct TestComponentB {
    int a;

    explicit TestComponentB(const int val): a(val) {}

    template<typename E, typename S>
    void operator()(E& e, const event::Cycle, Context<S>& ctx) const {
        ctx.template get_entity<E>();
        std::cout << "Cycle" " from B! " << a << std::endl;
    }
};

struct TestEntity {};

using TestEntity_t = Entity<TestEntity, TestComponentA>;

int main() {
    auto s = make_simulation()
            .add<TestEntity, TestComponentA>()
            .add<TestEntity, TestComponentA, TestComponentB>()
            .build();

    s.spawn<TestEntity_t>(1);
    s.spawn<Entity<TestEntity, TestComponentA, TestComponentB> >(555, 666);
    s.spawn<TestEntity_t>(2);
    s.run(10);

    std::cout << "Done" << std::endl;
    return 0;
}
