#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>

#include "Event.h"
#include "Concepts.h"
#include "Entity.h"
#include "Storage.h"
#include "Traits.h"

namespace sim {
    template<typename... Entities>
    class Simulation;

    // todo: concept is entity
    template<typename... Components>
    class Simulation final {
        Registry registry_;
        size_t cycle_ = 0;
        index_t entity_index_ = 0;

    public:
        [[nodiscard]] size_t cycle() const;

        void run(size_t for_cycles);

        EntityHandle create();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event);
    };

    class Context {
        Registry* registry_;

    public:
        explicit Context(Registry* registry);

        template<typename A, typename B>
        [[nodiscard]] View<A, B> view();
    };

    // Implementation ============================================================================

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        dispatch_to_all(event::SimStart{});
        for (size_t i = 0; i < for_cycles; ++i) {
            dispatch_to_all(event::Cycle{});
            ++cycle_;
        }
        dispatch_to_all(event::SimEnd{});
    }

    template<typename... Components>
    EntityHandle Simulation<Components...>::create() {
        return {entity_index_++, &registry_};
    }

    template<typename... Components>
    template<typename Event>
    void Simulation<Components...>::dispatch_to_all(const Event& event) {
        Context context(&registry_);
        (registry_.get<Components>().for_each([&](auto&& component) {
            // if constexpr (requires { component(*this, event, context); }) {
            //     component(*this, event, context);
            // }
            // if constexpr (requires { component(*this, event); }) {
            //     component(*this, event);
            // }
            if constexpr (requires { component(event, context); }) {
                component(event, context);
            }
            if constexpr (requires { component(event); }) {
                component(event);
            }
        }), ...);
    }

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename A, typename B>
    View<A, B> Context::view() {
        return {&registry_->get<A>(), &registry_->get<B>()};
    }
}

#endif //SIMULATION_H
