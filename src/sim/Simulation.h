#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>

#include "Event.h"

namespace sim {
    template<typename Item, typename... Collection>
    concept OneOf = (std::same_as<std::remove_cvref_t<Item>, Collection> || ...);

    template <typename... Entities>
    class Simulation {
    private:
        std::tuple<std::vector<Entities>...> entities_;

    public:
        template<typename Entity>
        requires OneOf<Entity, Entities...>
        void add(Entity&& entity);

        void run();

        template<typename Entity>
        std::vector<Entity> get_entities_of_type();
    };

    template<typename ... Entities>
    template<typename Entity>
    requires OneOf<Entity, Entities...>
    void Simulation<Entities...>::add(Entity&& entity) {
        std::get<std::vector<std::remove_cvref_t<Entity>>>(entities_).emplace_back(std::forward<Entity>(entity));
    }

    template<typename ... Entities>
    void Simulation<Entities...>::run() {
        (std::get<std::vector<Entities>>(entities_)[0].dispatch(event::Cycle{}, *this), ...);
    }

    template<typename ... Entities>
    template<typename Entity>
    std::vector<Entity> Simulation<Entities...>::get_entities_of_type() {
        return std::get<std::vector<Entity>>(entities_);
    }
}

#endif //SIMULATION_H
