#ifndef ENTITY_H
#define ENTITY_H
#include <tuple>

namespace sim {
    template<typename Component, typename Event>
    void try_dispatch(Component& component, const Event& event) {
        if constexpr (std::invocable<Component, Event>) {
            component(event);
        }
    }

    template <typename E, typename ...Components>
    struct Entity {
        std::tuple<Components...> components;

        template<typename... Args>
        explicit Entity(Args&&... args);

        template<typename Event>
        void dispatch(const Event& event);
    };

    template<typename E, typename ... Components>
    template<typename ... Args>
    Entity<E, Components...>::Entity(Args&&... args): components(std::forward<Args>(args)...) {
    }

    template<typename E, typename ... Components>
    template<typename Event>
    void Entity<E, Components...>::dispatch(const Event& event) {
        (try_dispatch(std::get<Components>(components), event), ...);
    }
}

#endif //ENTITY_H
