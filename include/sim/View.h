#ifndef VIEW_H
#define VIEW_H

#include "Storage.h"
#include "Traits.h"

namespace sim {
    template<typename... Components>
    class View {
        Registry* registry_;

    public:
        explicit View(Registry* registry);
        void for_each(auto&& func);
    };

    class Context {
        Registry* registry_;

    public:
        explicit Context(Registry* registry);

        template<typename... Components>
        [[nodiscard]] View<Components...> view();
    };

    // Implementation ============================================================================

    template<typename... Components>
    View<Components...>::View(Registry* registry): registry_(registry) {}

    template<typename... Components>
    void View<Components...>::for_each(auto&& func) {
        auto& storage = registry_->get_storage<first_t<Components...> >();
        storage.for_each([&](const id_t id, auto&) {
            const bool has_all = (... && registry_->get_storage<Components>().entity_has(id));
            if (has_all) {
                EntityHandle entity(id, registry_);
                if constexpr (requires { func(entity, registry_->get_storage<Components>().entity_get(id)...); })
                    std::forward<decltype(func)>(func)(entity, registry_->get_storage<Components>().entity_get(id)...);
                else
                    std::forward<decltype(func)>(func)(registry_->get_storage<Components>().entity_get(id)...);
            }
        });
    }

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename... Components>
    View<Components...> Context::view() {
        return registry_->view<Components...>();
    }
}
#endif //VIEW_H
