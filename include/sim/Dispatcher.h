#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "View.h"

namespace sim {
    template<typename... Ss>
    class Systems {};

    template<typename... Ss>
    class Dispatcher {
        std::tuple<Ss...> systems_;

    public:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& context);
    };

    template<typename... Ss>
    template<typename Event>
    void Dispatcher<Ss...>::dispatch_to_all(const Event& event, Context& context) {
        std::apply([&](Ss&... system) {
            ([&] {
                if constexpr (requires { system(event); }) {
                    system(event);
                }
                if constexpr (requires { system(event, context); }) {
                    system(event, context);
                }
            }(), ...);
        }, systems_);
    }
}
#endif //SYSTEMS_H
