#ifndef VIEW_H
#define VIEW_H
#include "Storage.h"

namespace sim {
    class Context {
        Registry* registry_;

    public:
        explicit Context(Registry* registry);

        template<typename A, typename B>
        [[nodiscard]] View<A, B> view();
    };

    inline Context::Context(sim::Registry* registry): registry_(registry) {}

    template<typename A, typename B>
    View<A, B> Context::view() {
        return {&registry_->get<A>(), &registry_->get<B>()};
    }
}
#endif //VIEW_H
