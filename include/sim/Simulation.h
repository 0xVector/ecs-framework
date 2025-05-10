#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>

#include "Event.h"
#include "Concepts.h"
#include "Entity.h"
#include "Traits.h"

namespace sim {
    template<typename... Entities>
    class Simulation;

    template<typename Sim> // TODO: concept
    class Context {
        using sim_t = extracted_to_t<Sim, Simulation>;
        sim_t* simulation_;

    public:
        explicit Context(sim_t* simulation);

        template<typename E>
        // requires OneOf<E, Entities...> // TODO
        E& get_entity();

        [[nodiscard]] size_t cycle() const;
    };

    // todo: concept is entity
    template<typename... Entities>
    class Simulation final {
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

        template<typename E>
        E& get_entity();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event, Context<Simulation>& context);
    };

    template<typename... Es>
    class Builder {
    public:
        template<typename EntityType, typename... Components>
        constexpr auto add() const { // TODO: unique Components
            return Builder<Es..., Entity<EntityType, Components...> >();
        }

        constexpr Simulation<Es...> build() { return Simulation<Es...>(); }
    };

    constexpr Builder<> make_simulation() { return {}; }

    // Implementation ============================================================================

    template<typename Sim>
    Context<Sim>::Context(sim_t* simulation): simulation_(simulation) {}

    template<typename Sim>
    template<typename E>
    E& Context<Sim>::get_entity() {
        return simulation_->template get_entity<E>();
    }

    template<typename Sim>
    size_t Context<Sim>::cycle() const {
        return simulation_->cycle();
    }

    template<typename... Entities>
    Simulation<Entities...>::~Simulation() = default;

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        Context<Simulation> context(this);

        dispatch_to_all(event::SimStart{}, context);
        for (size_t i = 0; i < for_cycles; ++i) {
            dispatch_to_all(event::Cycle{}, context);
            ++cycle_;
        }
        dispatch_to_all(event::SimEnd{}, context);
    }

    template<typename... Entities>
    template<typename E, typename... Args> requires OneOf<E, Entities...>
    auto Simulation<Entities...>::spawn(Args&&... args) {
        return std::get<E>(entities_).spawn(std::forward<Args>(args)...);
    }

    template<typename... Entities>
    template<typename E>
    E& Simulation<Entities...>::get_entity() {
        return std::get<E>(entities_);
    }

    template<typename... Entities>
    template<typename Event>
    void Simulation<Entities...>::dispatch_to_all(const Event& event, Context<Simulation>& context) {
        (std::get<Entities>(entities_).dispatch(event, context), ...);
    }
}

#endif //SIMULATION_H
