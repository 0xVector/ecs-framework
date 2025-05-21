#ifndef VIEW_H
#define VIEW_H
#include "Storage.h"

namespace sim {
    template<typename A, typename B>
    class View {
        Storage<A>* storage_a_;
        Storage<B>* storage_b_;

    public:
        View(Storage<A>* storage_a, Storage<B>* storage_b);
        void for_each(auto&& func);
    };

    class Context {
        Registry* registry_;

    public:
        explicit Context(Registry* registry);

        template<typename A, typename B>
        [[nodiscard]] View<A, B> view();
    };

    // Implementation ============================================================================

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename A, typename B>
    View<A, B> Context::view() {
        return {&registry_->get<A>(), &registry_->get<B>()};
    }

    template<typename A, typename B>
    View<A, B>::View(Storage<A>* storage_a, Storage<B>* storage_b): storage_a_(storage_a), storage_b_(storage_b) {}

    template<typename A, typename B>
    void View<A, B>::for_each(auto&& func) {
        storage_a_->for_each([&](auto&& a) {
            storage_b_->for_each([&](auto&& b) {
                func(a, b);
            });
        });
    }
}
#endif //VIEW_H
