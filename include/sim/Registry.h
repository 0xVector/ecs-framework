#ifndef REGISTRY_H
#define REGISTRY_H
#include "Storage.h"

namespace sim {
    template<typename... Cs>
    class Components {};

    using component_id_t = size_t;

    inline component_id_t generate_component_id() {
        static component_id_t id = 0;
        return id++;
    }

    template<typename C>
    component_id_t get_component_id() {
        static component_id_t id = generate_component_id();
        return id;
    }

    class Entity;

    // Forward declaration (TODO)
    template<typename... Cs>
    class View;

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename C>
        [[nodiscard]] Storage<C>& get_storage();

        template<typename C>
        void push_back(Entity entity, C&& component);

        template<typename Component, typename... Args>
        void emplace(Entity entity, Args&&... args);

        template<typename... Cs>
        [[nodiscard]] View<Cs...> view();
    };

    class [[nodiscard]] Entity {
        id_t id_;
        Registry* registry_;

    public:
        Entity(id_t id, Registry* registry);

        [[nodiscard]] id_t id() const;

        template<typename Component>
        [[nodiscard]] bool has() const;

        template<typename Component>
        [[nodiscard]] Component& get() const;

        template<typename C>
        Entity& push_back(C&& component);

        template<typename Component, typename... Args>
        Entity& emplace(Args&&... args);
    };

    // Implementation ============================================================================

    template<typename C>
    Storage<C>& Registry::get_storage() {
        auto id = get_component_id<C>();
        if (id >= storages_.size()) {
            storages_.resize(id + 1);
            storages_[id] = std::make_unique<Storage<C> >();
        }
        return static_cast<Storage<C> &>(*storages_[id]);
    }

    template<typename Component>
    void Registry::push_back(const Entity entity, Component&& component) {
        auto& storage = get_storage<std::decay_t<Component> >();
        storage.push_back(entity.id(), std::forward<Component>(component));
    }

    template<typename Component, typename... Args>
    void Registry::emplace(const Entity entity, Args&&... args) {
        auto& storage = get_storage<Component>();
        storage.emplace(entity.id(), std::forward<Args>(args)...);
    }

    template<typename... Cs>
    View<Cs...> Registry::view() {
        return View<Cs...>(this);
    }

    inline Entity::Entity(const id_t id, Registry* registry): id_(id), registry_(registry) {}

    inline id_t Entity::id() const {
        return id_;
    }

    template<typename Component>
    bool Entity::has() const {
        return registry_->get_storage<Component>().entity_has(id_);
    }

    template<typename Component>
    Component& Entity::get() const {
        return registry_->get_storage<Component>().entity_get(id_);
    }

    template<typename Component>
    Entity& Entity::push_back(Component&& component) {
        registry_->push_back(*this, std::forward<Component>(component));
        return *this;
    }

    template<typename Component, typename... Args>
    Entity& Entity::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
        return *this;
    }
};
#endif //REGISTRY_H
