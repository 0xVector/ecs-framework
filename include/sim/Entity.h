#ifndef ENTITY_H
#define ENTITY_H
#include <tuple>

#include "Simulation.h"

namespace sim {
    // TODO: is empty concept on Tag
    template<typename Tag, typename... Components>
    class Entity {
        std::tuple<std::vector<Components>...> components_;
        size_t size_ = 0;

    public:
        using id_t = size_t;
        Entity() = default;

        template<typename... Args>
        id_t spawn(Args&&... args);


        template<typename Event>
        void dispatch(const Event& event, Context& context);

    private:
        template<typename Component, typename Event>
        void try_dispatch(Component& component, const Event& event, Context& context);
    };

    // Implementation ============================================================================

    template<typename Tag, typename... Components>
    template<typename... Args>
    typename Entity<Tag, Components...>::id_t Entity<Tag, Components...>::spawn(Args&&... args) {
        std::get<std::vector<Components> >(components_).emplace_back(std::forward<Args>(args)...);
        return size_++;
    }

    template<typename Tag, typename ... Components>
    template<typename Event>
    void Entity<Tag, Components...>::dispatch(const Event& event, Context& context) {
        std::apply([&](std::vector<Components>&... components) {
            (std::ranges::for_each(components, [&](auto& component) {
                try_dispatch(component, event, context);
            }), ...);
        }, components_);
    }

    template<typename Tag, typename ... Components>
    template<typename Component, typename Event>
    void Entity<Tag, Components...>::try_dispatch(Component& component, const Event& event, Context& context) {
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
