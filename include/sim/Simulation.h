#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>
#include <algorithm>

#include "Event.h"
#include "Concepts.h"

namespace sim {
    struct Context {
        template<typename E, typename Self>
        std::vector<E>& do_get_entities(this Self&& self) {
            return self.template get_entities<E>();
        }
    };

    template<typename... Entities>
    class Simulation {
    public:
        struct SimulationContext;

    private:
        std::tuple<Entities...> entities_;
        size_t cycle_ = 0;

    public:
        Simulation() = default;
        virtual ~Simulation();

        [[nodiscard]] size_t cycle() const;

        void run(size_t for_cycles);

        template<typename E, typename... Args>
            requires OneOf<E, Entities...>
        auto spawn(Args&&... args);

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event, SimulationContext&context);
    };

    // Implementation ============================================================================

    template<typename... Entities>
    Simulation<Entities...>::~Simulation() = default;

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        SimulationContext context{entities()};

        entities_.dispatch_to_all(event::SimStart{}, context);
        for (size_t i = 0; i < for_cycles; ++i) {
            entities_.dispatch_to_all(event::Cycle{}, context);
            ++cycle_;
        }
        entities_.dispatch_to_all(event::SimEnd{}, context);
    }

    template<typename ... Entities>
    template<typename E, typename ... Args> requires OneOf<E, Entities...>
    auto Simulation<Entities...>::spawn(Args&&... args) {
        return std::get<std::vector<E> >(entities_).emplace_back(std::forward<Args>(args)...);
    }

    template<typename... Entities>
    template<typename Event>
    void Simulation<Entities...>::EntityStorage::dispatch_to_all(const Event& event,
                                                                 typename Simulation<Entities...>::SimulationContext&
                                                                 context) {
        (std::ranges::for_each(std::get<std::vector<Entities> >(entities_),
                               [&](auto&& e) { e.dispatch(event, context); }), ...);
        // std::apply([&event, this](auto&&... entity_vect) {
        //     (std::ranges::for_each(entity_vect, [&](auto&& e) { e.dispatch(event, *this); }), ...);
        // }, entities_);
    }
}

#endif //SIMULATION_H
