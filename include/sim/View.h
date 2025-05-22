#ifndef VIEW_H
#define VIEW_H
#include "Storage.h"

namespace sim {
    template<typename... Components>
    class View {
        std::tuple<Storage<Components>*...> storages_;

    public:
        explicit View(Storage<Components>*... storages);
        void for_each(auto&& func);

    private:
        template<std::size_t I = 0, typename ... Cs>
        void for_each_nested(auto&& func, Cs&... components);
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
    View<Components...>::View(Storage<Components>*... storages): storages_(storages...) {}

    template<typename... Components>
    void View<Components...>::for_each(auto&& func) {
        for_each_nested(std::forward<decltype(func)>(func));

        // storage_a_->for_each([&](auto&& a) {
        //     storage_b_->for_each([&](auto&& b) {
        //         func(a, b);
        //     });
        // });
    }

    template<typename ... Components>
    template<std::size_t I, typename ... Cs>
    void View<Components...>::for_each_nested(auto&& func, Cs&... components) {
        if constexpr (I < sizeof...(Components)) {
            std::get<I>(storages_)->for_each([&](auto& component) {
                for_each_nested<I + 1>(func, components..., component);
            });
        } else {
            func(components...);
        }
    }

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename ... Components>
    View<Components...> Context::view() {
        return View<Components...>(&registry_->get<Components>()...);
    }
}
#endif //VIEW_H
