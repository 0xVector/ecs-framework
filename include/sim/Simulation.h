#ifndef SIMULATION_H
#define SIMULATION_H

#include "Event.h"
#include "Storage.h"
#include "Dispatcher.h"

/// @brief The main namespace for the simulation framework.
namespace sim {

    /// @brief The main simulation class that manages the lifecycle of the simulation.
    /// @tparam Ss The systems that will be used in the simulation.
    template<typename... Ss>
    class Simulation final {
        static constexpr size_t COMPACTION_CYCLES = 100;
        Registry registry_;
        Dispatcher<Ss...> dispatcher_{};
        size_t cycle_ = 0;
        id_t entity_id_ = 0;

    public:
        /// @brief Default constructor for the Simulation class.
        explicit Simulation() = default;

        /// @brief A fluent interface to add systems to the simulation.
        /// @tparam S The system types to add.
        /// @return A new Simulation instance with the added systems.
        template<typename... S>
        constexpr auto with_systems() const {
            return Simulation<Ss..., S...>{};
        }

        /// @brief Returns the current simulation cycle.
        /// @return The current cycle number.
        [[nodiscard]] size_t cycle() const;

        /// @brief Runs the simulation for a specified number of cycles.
        /// @param cycles The number of cycles to run the simulation.
        void run(size_t cycles);

        /// @brief Creates a new entity in the simulation.
        /// @return A new Entity object representing the created entity.
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
