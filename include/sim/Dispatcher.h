#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "View.h"

namespace sim {
    /// @brief Dispatcher class that allows dispatching events to multiple systems.
    /// @tparam Ss Variadic template parameter for systems.
    template<typename... Ss>
    class Dispatcher {
        std::tuple<Ss...> systems_;

    public:
        /// @brief Dispatch an event to all systems.
        /// @tparam Event The event to be dispatched.
        /// @param event The event to be dispatched.
        /// @param context The context in which the event is dispatched.
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
