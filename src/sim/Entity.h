#ifndef ENTITY_H
#define ENTITY_H
#include <tuple>

#include "Simulation.h"

namespace sim {
    template<typename E, typename... Components>
    struct Entity {
        std::tuple<Components...> components;

        template<typename... Args>
        explicit Entity(Args&&... args);

        template<typename Event, typename... Entities>
        void dispatch(const Event& event, Simulation<Entities...>& simulation);

    private:
        template<typename Component, typename Event, typename... Entities>
        static void try_dispatch(Component& component, const Event& event, Simulation<Entities...>& simulation);
    };

    template<typename E, typename... Components>
    template<typename... Args>
    Entity<E, Components...>::Entity(Args&&... args):
        components(std::forward<Args>(args)...) {
    }

    template<typename E, typename... Components>
    template<typename Event, typename... Entities>
    void Entity<E, Components...>::dispatch(const Event& event, Simulation<Entities...>& simulation) {
        (try_dispatch(std::get<Components>(components), event, simulation), ...);
    }

    template<typename E, typename... Components>
    template<typename Component, typename Event, typename... Entities>
    void Entity<E, Components...>::try_dispatch(Component& component, const Event& event,
                                                Simulation<Entities...>& simulation) {
        if constexpr (std::invocable<Component, Event, Simulation<Entities...> &>) {
            component(event, simulation);
        }
        if constexpr (std::invocable<Component, Event>) {
            component(event);
        }
    }
}

#endif //ENTITY_H
