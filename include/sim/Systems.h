#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "View.h"
#include "Storage.h"

namespace sim {
    template<typename... Sys>
    class Systems {};

    template<typename... Systems>
    class Dispatcher {
        std::tuple<Systems...> systems_;

    public:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& context, Registry& registry_);
    };

    template<typename... Systems>
    template<typename Event>
    void Dispatcher<Systems...>::dispatch_to_all(const Event& event, Context& context, Registry& registry) {
        (registry.get<Cs>().for_each([&](auto&& component) {
            // if constexpr (requires { component(*this, event, context); }) {
            //     component(*this, event, context);
            // }
            // if constexpr (requires { component(*this, event); }) {
            //     component(*this, event);
            // }
            if constexpr (requires { component(event, context); }) {
                component(event, context);
            }
            if constexpr (requires { component(event); }) {
                component(event);
            }
        }), ...);
    }
}
#endif //SYSTEMS_H
