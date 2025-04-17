#ifndef SIMULATION_H
#define SIMULATION_H
#include <tuple>
#include <vector>
#include <bits/ranges_algo.h>

#include "Event.h"
#include "Concepts.h"

namespace sim {
    class Context;

    template<typename... Entities>
    class EntityStorage {
    private:
        std::tuple<std::vector<Entities>...> entities_;

    public:
        template<typename Entity>
        requires OneOf<Entity, Entities...>
        const std::vector<Entity>& get_all() const;

        template<typename Entity>
        requires OneOf<Entity, Entities...>
        std::vector<Entity>& get_all();

        template<typename Entity>
        requires OneOf<Entity, Entities...>
        void add(Entity&& entity);

        template<typename Event>
        void dispatch_to_all(const Event& event, Context& context);
    };

    class EntityAccessorBase {
        // virtual ~EntityAccessorBase() = default;
    };

    class Context final {
    private:
        EntityAccessorBase& entity_accessor_;

    public:
        explicit Context(EntityAccessorBase& entity_accessor);

        EntityAccessorBase& entities();
    };

    inline Context::Context(EntityAccessorBase& entity_accessor): entity_accessor_(entity_accessor) {
    }

    template<typename... Entities>
    class Simulation {
    private:
        using entity_storage_t = EntityStorage<Entities...>;
        struct EntityAccessor;

        size_t cycle_ = 0;
        entity_storage_t entities_;
        EntityAccessor entity_accessor_;

    public:
        Simulation();
        virtual ~Simulation();

        [[nodiscard]] size_t cycle() const;

        entity_storage_t& entities();

        void run(size_t for_cycles);
    };

    template<typename... Entities>
    struct Simulation<Entities...>::EntityAccessor final : EntityAccessorBase {
    private:
        entity_storage_t& storage_;

    public:
        explicit EntityAccessor(entity_storage_t& storage);

        template<typename T>
        requires OneOf<T, Entities...>
        std::vector<T> get_all();
    };

    // Implementation ============================================================================

    template<typename ... Entities>
    template<typename Entity> requires OneOf<Entity, Entities...>
    const std::vector<Entity>& EntityStorage<Entities...>::get_all() const {
        return std::get<std::vector<Entity> >(entities_);
    }

    template<typename ... Entities>
    template<typename Entity> requires OneOf<Entity, Entities...>
    std::vector<Entity>& EntityStorage<Entities...>::get_all() {
        return std::get<std::vector<Entity> >(entities_);
    }

    template<typename ... Entities>
    template<typename Entity> requires OneOf<Entity, Entities...>
    void EntityStorage<Entities...>::add(Entity&& entity) {
        std::get<std::vector<std::remove_cvref_t<Entity> > >(entities_).emplace_back(std::forward<Entity>(entity));
    }

    template<typename ... Entities>
    template<typename Event>
    void EntityStorage<Entities...>::dispatch_to_all(const Event& event, Context& context) {
        (std::ranges::for_each(std::get<std::vector<Entities> >(entities_),
                       [&](auto&& e) { e.dispatch(event, context); }), ...);
        // std::apply([&event, this](auto&&... entity_vect) {
        //     (std::ranges::for_each(entity_vect, [&](auto&& e) { e.dispatch(event, *this); }), ...);
        // }, entities_);
    }

    template<typename ... Entities>
    Simulation<Entities...>::Simulation(): entity_accessor_(entities_) {
    }

    template<typename ... Entities>
    Simulation<Entities...>::~Simulation() = default;

    template<typename... Entities>
    size_t Simulation<Entities...>::cycle() const {
        return cycle_;
    }

    template<typename ... Entities>
    typename Simulation<Entities...>::entity_storage_t& Simulation<Entities...>::entities() {
        return entities_;
    }

    template<typename... Entities>
    void Simulation<Entities...>::run(const size_t for_cycles) {
        Context context{entity_accessor_};

        entities_.dispatch_to_all(event::SimStart{}, context);
        for (size_t i = 0; i < for_cycles; ++i) {
            entities_.dispatch_to_all(event::Cycle{}, context);
            ++cycle_;
        }
        entities_.dispatch_to_all(event::SimEnd{}, context);
    }

    template<typename ... Entities>
    Simulation<Entities...>::EntityAccessor::EntityAccessor(entity_storage_t& storage): storage_(storage) {
    }

    template<typename ... Entities>
    template<typename T> requires OneOf<T, Entities...>
    std::vector<T> Simulation<Entities...>::EntityAccessor::get_all() {
        return storage_.template get_all<T>();
    }
}

#endif //SIMULATION_H
