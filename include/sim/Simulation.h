#ifndef SIMULATION_H
#define SIMULATION_H

#include "Event.h"
#include "Storage.h"
#include "Dispatcher.h"

namespace sim {
    template<typename... Ss>
    class Simulation final {
        static constexpr size_t COMPACTION_CYCLES = 100;
        Registry registry_;
        Dispatcher<Ss...> dispatcher_{};
        size_t cycle_ = 0;
        id_t entity_id_ = 0;

    public:
        template<typename... S>
        constexpr auto with_systems() const {
            return Simulation<Ss..., S...>{};
        }

        [[nodiscard]] size_t cycle() const;

        void run(size_t cycles);

        Entity create();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context& ctx);

        void compact_storages();
    };

    // Implementation ============================================================================

    template<typename... Ss>
    size_t Simulation<Ss...>::cycle() const {
        return cycle_;
    }

    template<typename... Ss>
    void Simulation<Ss...>::run(const size_t cycles) {
        Context start_ctx(&registry_, cycle_);
        dispatch_to_all(event::SimStart{}, start_ctx);

        for (size_t i = 0; i < cycles; ++i) {
            Context ctx(&registry_, cycle_);
            dispatch_to_all(event::PreCycle{}, ctx);
            dispatch_to_all(event::Cycle{}, ctx);
            dispatch_to_all(event::PostCycle{}, ctx);
            dispatch_to_all(event::Render{}, ctx);
            compact_storages();
            ++cycle_;
        }

        Context end_ctx(&registry_, cycle_);
        dispatch_to_all(event::SimEnd{}, end_ctx);
    }

    template<typename... Ss>
    Entity Simulation<Ss...>::create() {
        return {entity_id_++, &registry_};
    }

    template<typename... Ss>
    template<typename Event>
    void Simulation<Ss...>::dispatch_to_all(const Event& event, Context& ctx) {
        dispatcher_.template dispatch_to_all<Event>(event, ctx);
    }

    template<typename... Ss>
    void Simulation<Ss...>::compact_storages() {
        if (cycle_ % COMPACTION_CYCLES == 0)
            registry_.compact_all();
    }
}

#endif //SIMULATION_H
