#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "View.h"

namespace sim {
    template<typename... Sys>
    class SystemsPack {};

    template<typename... Systems>
    class Dispatcher {
        std::tuple<Systems...> systems_;

    public:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& context);
    };

    template<typename... Systems>
    template<typename Event>
    void Dispatcher<Systems...>::dispatch_to_all(const Event& event, Context& context) {
        std::apply([&](Systems&... system) {
            ([&]() {
                // if constexpr (requires { component(*this, event, context); }) {
                //     component(*this, event, context);
                // }
                // if constexpr (requires { component(*this, event); }) {
                //     component(*this, event);
                // }
                if constexpr (requires { system(event, context); }) {
                    system(event, context);
                }
                if constexpr (requires { system(event); }) {
                    system(event);
                }
            }(), ...);
        }, systems_);
    }
}
#endif //SYSTEMS_H
