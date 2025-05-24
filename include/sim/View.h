#ifndef VIEW_H
#define VIEW_H

#include "Registry.h"
#include "Traits.h"

namespace sim {
    template<typename... Cs>
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

        template<typename... Cs>
        [[nodiscard]] View<Cs...> view();
    };

    // Implementation ============================================================================

    template<typename... Cs>
    View<Cs...>::View(Registry* registry): registry_(registry) {}

    template<typename... Cs>
    void View<Cs...>::for_each(auto&& func) {
        auto& storage = registry_->get_storage<first_t<Cs...> >();
        storage.for_each([&](const id_t id, auto&) {
            const bool has_all = (... && registry_->get_storage<Cs>().entity_has(id));
            if (has_all) {
                Entity entity(id, registry_);
                if constexpr (requires { func(entity, registry_->get_storage<Cs>().entity_get(id)...); })
                    std::forward<decltype(func)>(func)(entity, registry_->get_storage<Cs>().entity_get(id)...);
                else
                    std::forward<decltype(func)>(func)(registry_->get_storage<Cs>().entity_get(id)...);
            }
        });
    }

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename... Cs>
    View<Cs...> Context::view() {
        return registry_->view<Cs...>();
    }
}
#endif //VIEW_H
