#ifndef SIMULATION_H
#define SIMULATION_H

#include "Event.h"
#include "Storage.h"
#include "Systems.h"

namespace sim {
    template<typename Components, typename Systems>
    class Simulation;

    // todo: concept is entity
    template< //template<typename...> class Components, template<typename...> class Systems,
        typename... Cs, typename... Ss>
    class Simulation<ComponentsPack<Cs...>, SystemsPack<Ss...> > final {
        Registry registry_;
        Dispatcher<Ss...> dispatcher_{};
        size_t cycle_ = 0;
        index_t entity_index_ = 0;

    public:
        template<typename... C>
        constexpr auto with_components() const {
            return Simulation<ComponentsPack<Cs..., C...>, SystemsPack<Ss...> >{};
        }

        template<typename... S>
        constexpr auto with_systems() const {
            return Simulation<ComponentsPack<Cs...>, SystemsPack<Ss..., S...> >{};
        }

        [[nodiscard]] size_t cycle() const;

        void run(size_t for_cycles);

        EntityHandle create();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& ctx);
    };

    constexpr auto make_simulation() {
        return Simulation<ComponentsPack<>, SystemsPack<> >();
    }

    // Implementation ============================================================================

    template<typename... Cs, typename... Ss>
    size_t Simulation<ComponentsPack<Cs...>, SystemsPack<Ss...> >::cycle() const {
        return cycle_;
    }

    template<typename... Cs, typename... Ss>
    void Simulation<ComponentsPack<Cs...>, SystemsPack<Ss...> >::run(const size_t for_cycles) {
        Context ctx(&registry_);
        dispatch_to_all(event::SimStart{}, ctx);
        for (size_t i = 0; i < for_cycles; ++i) {
            dispatch_to_all(event::Cycle{}, ctx);
            ++cycle_;
        }
        dispatch_to_all(event::SimEnd{}, ctx);
    }

    template<typename... Cs, typename... Ss>
    EntityHandle Simulation<ComponentsPack<Cs...>, SystemsPack<Ss...> >::create() {
        return {entity_index_++, &registry_};
    }

    template<typename... Cs, typename... Ss>
    template<typename Event>
    void Simulation<ComponentsPack<Cs...>, SystemsPack<Ss...> >::dispatch_to_all(const Event& event, Context& ctx) {
        dispatcher_.template dispatch_to_all<Event>(event, ctx);
    }
}

#endif //SIMULATION_H
