#ifndef ENTITY_H
#define ENTITY_H
#include <tuple>

#include "Simulation.h"

namespace sim {
    template<typename E, typename... Components>
    struct Entity {
        using simulation_t = Simulation<Components...>;

        std::tuple<Components...> components;

        template<typename... Args>
        explicit Entity(Args&&... args);

        // template<typename Event, typename... Entities>
        // void dispatch(const Event& event, simulation_t& simulation);

        template<typename Event>
        void dispatch(const Event& event, Context& context);

        template<typename Component>
        requires OneOf<Component, Components...>
        Component get_component();

    private:
        // template<typename Component, typename Event, typename... Entities>
        // static void try_dispatch(Component& component, const Event& event, simulation_t& simulation);

        template<typename Component, typename Event>
        void try_dispatch(Component& component, const Event& event, Context& context);
    };

    // Implementation ============================================================================

    template<typename E, typename... Components>
    template<typename... Args>
    Entity<E, Components...>::Entity(Args&&... args):
        components(std::forward<Args>(args)...) {
    }

    template<typename E, typename ... Components>
    template<typename Event>
    void Entity<E, Components...>::dispatch(const Event& event, Context& context) {
        (try_dispatch(std::get<Components>(components), event, context), ...);
    }

    template<typename E, typename... Components>
    template<typename Component> requires OneOf<Component, Components...>
    Component Entity<E, Components...>::get_component() {
        return std::get<Component>(components);
    }

    template<typename E, typename ... Components>
    template<typename Component, typename Event>
    void Entity<E, Components...>::try_dispatch(Component& component, const Event& event, Context& context) {
        if constexpr (requires {component(*this, event, context);}) {
            component(*this, event, context);
        }
        if constexpr (requires {component(*this, event);}) {
            component(*this, event);
        }
        if constexpr (requires {component(event, context);}) {
            component(event, context);
        }
        if constexpr (requires {component(event);}) {
            component(event);
        }
    }
}

#endif //ENTITY_H
