#ifndef ENTITY_H
#define ENTITY_H
#include <tuple>

#include "Simulation.h"

namespace sim {
    struct Position {
        int x;
        int y;

        Position():
            x(0), y(0) {
        }

        Position(const int x, const int y):
            x(x), y(y) {
        }
    };

    template<typename E, typename... Components>
    struct Entity {
        Position position;
        std::tuple<Components...> components;

        template<typename... Args>
        explicit Entity(Args&&... args);

        template<typename... Args>
        explicit Entity(Position position, Args&&... args);

        template<typename Event, typename... Entities>
        void dispatch(const Event& event, Simulation<Entities...>& simulation);

        template<typename Component>
            requires OneOf<Component, Components...>
        Component get_component();

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
    template<typename... Args>
    Entity<E, Components...>::Entity(Position position, Args&&... args):
        position(position), components(std::forward<Args>(args)...) {
    }

    template<typename E, typename... Components>
    template<typename Event, typename... Entities>
    void Entity<E, Components...>::dispatch(const Event& event, Simulation<Entities...>& simulation) {
        (try_dispatch(std::get<Components>(components), event, simulation), ...);
    }

    template<typename E, typename... Components>
    template<typename Component> requires OneOf<Component, Components...>
    Component Entity<E, Components...>::get_component() {
        return std::get<Component>(components);
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
