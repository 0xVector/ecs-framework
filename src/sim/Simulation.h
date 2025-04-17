#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>
#include <bits/ranges_algo.h>

#include "Event.h"

namespace sim {
    template<typename Item, typename... Collection>
    concept OneOf = (std::same_as<std::remove_cvref_t<Item>, Collection> || ...);

    template<typename... Entities>
    class Simulation {
    private:
        size_t cycle_ = 0;
        std::tuple<std::vector<Entities>...> entities_;

    public:
        [[nodiscard]] size_t cycle() const;

        template<typename Entity>
            requires OneOf<Entity, Entities...>
        void add(Entity&& entity);

        void run(size_t for_cycles);

        template<typename Entity>
        std::vector<Entity> get_entities_of_type();

    private:
        template<typename Event>
        void dispatch_to_all(const Event& event);
    };

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename... Entities>
    template<typename Entity>
        requires OneOf<Entity, Entities...>
    void Simulation<Entities...>::add(Entity&& entity) {
        std::get<std::vector<std::remove_cvref_t<Entity> > >(entities_).emplace_back(std::forward<Entity>(entity));
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        dispatch_to_all(event::SimStart{});
        for (size_t i = 0; i < for_cycles; ++i) {
            dispatch_to_all(event::Cycle{});
            ++cycle_;
        }
        dispatch_to_all(event::SimEnd{});
    }

    template<typename... Entities>
    template<typename Entity>
    std::vector<Entity> Simulation<Entities...>::get_entities_of_type() {
        return std::get<std::vector<Entity> >(entities_);
    }

    template<typename... Entities>
    template<typename Event>
    void Simulation<Entities...>::dispatch_to_all(const Event& event) {
        (std::ranges::for_each(std::get<std::vector<Entities> >(entities_),
                               [&](auto&& e) { e.dispatch(event, *this); }), ...);
        // std::apply([&event, this](auto&&... entity_vect) {
        //     (std::ranges::for_each(entity_vect, [&](auto&& e) { e.dispatch(event, *this); }), ...);
        // }, entities_);
    }
}

#endif //SIMULATION_H
