#ifndef SIMULATION_H
#define SIMULATION_H

#include "Event.h"
#include "Storage.h"
#include "Dispatcher.h"

namespace sim {
    template<typename Components, typename Systems>
    class Simulation;

    template<typename... Cs, typename... Ss>
    class Simulation<Components<Cs...>, Systems<Ss...> > final {
        Registry registry_;
        Dispatcher<Ss...> dispatcher_{};
        size_t cycle_ = 0;
        id_t entity_id_ = 0;

    public:
        template<typename... C>
        constexpr auto with_components() const {
            return Simulation<Components<Cs..., C...>, Systems<Ss...> >{};
        }

        template<typename... S>
        constexpr auto with_systems() const {
            return Simulation<Components<Cs...>, Systems<Ss..., S...> >{};
        }

        [[nodiscard]] size_t cycle() const;

        void run(size_t for_cycles);

        Entity create();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& ctx);
    };

    constexpr auto make_simulation() {
        return Simulation<Components<>, Systems<> >();
    }

    // Implementation ============================================================================

    template<typename... Cs, typename... Ss>
    size_t Simulation<Components<Cs...>, Systems<Ss...> >::cycle() const {
        return cycle_;
    }

    template<typename... Cs, typename... Ss>
    void Simulation<Components<Cs...>, Systems<Ss...> >::run(const size_t for_cycles) {
        Context ctx(&registry_);
        dispatch_to_all(event::SimStart{}, ctx);
        for (size_t i = 0; i < for_cycles; ++i) {
            dispatch_to_all(event::PreCycle{}, ctx);
            dispatch_to_all(event::Cycle{}, ctx);
            dispatch_to_all(event::PostCycle{}, ctx);
            ++cycle_;
        }
        dispatch_to_all(event::SimEnd{}, ctx);
    }

    template<typename... Cs, typename... Ss>
    Entity Simulation<Components<Cs...>, Systems<Ss...> >::create() {
        return {entity_id_++, &registry_};
    }

    template<typename... Cs, typename... Ss>
    template<typename Event>
    void Simulation<Components<Cs...>, Systems<Ss...> >::dispatch_to_all(const Event& event, Context& ctx) {
        dispatcher_.template dispatch_to_all<Event>(event, ctx);
    }
}

#endif //SIMULATION_H
