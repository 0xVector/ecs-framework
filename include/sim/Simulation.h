#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>

#include "Event.h"
#include "Concepts.h"
#include "Entity.h"
#include "Storage.h"
#include "Traits.h"

namespace sim {
    template<typename... Entities>
    class Simulation;

    // todo: concept is entity
    template<typename... Components>
    class Simulation final {
        Registry<Components...> storage_;
        size_t cycle_ = 0;
        index_t entity_index_ = 0;

    public:
        [[nodiscard]] size_t cycle() const;

        void run(size_t for_cycles);

        EntityHandle<Components...> create();
    };

    // Implementation ============================================================================

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        storage_.dispatch(event::SimStart{});
        for (size_t i = 0; i < for_cycles; ++i) {
            storage_.dispatch(event::Cycle{});
            ++cycle_;
        }
        storage_.dispatch(event::SimEnd{});
    }

    template<typename ... Components>
    EntityHandle<Components...> Simulation<Components...>::create() {
        return {entity_index_++, &storage_};
    }
}

#endif //SIMULATION_H
