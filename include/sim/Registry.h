#ifndef REGISTRY_H
#define REGISTRY_H
#include "Storage.h"

namespace sim {
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

    // Forward declarations and type aliases ================================================================
    template<bool Const>
    class EntityBase;

    using Entity = EntityBase<false>;
    using ConstEntity = EntityBase<true>;

    // Forward declaration (TODO)
    template<bool Imm, typename... Cs>
    class View;

    template<typename... Cs>
    using ImmutableView = View<true, Cs...>;

    template<typename... Cs>
    using MutableView = View<false, Cs...>;

    // ======================================================================================================

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename C>
        [[nodiscard]] const Storage<C>& get_storage() const;

        template<typename C>
        [[nodiscard]] Storage<C>& get_storage();

        template<typename C>
        void push_back(ConstEntity entity, C&& component);

        template<typename Component, typename... Args>
        void emplace(ConstEntity entity, Args&&... args);

        void remove(ConstEntity entity);

        template<typename... Cs>
        [[nodiscard]] ImmutableView<Cs...> view() const;

        template<typename... Cs>
        [[nodiscard]] MutableView<Cs...> view();

        void compact_all();
    };

    template<bool Const>
    class [[nodiscard]] EntityBase {
        id_t id_;
        Registry* registry_;

    public:
        EntityBase() = default;
        EntityBase(id_t id, Registry* registry);

        operator EntityBase<true>() const; // NOLINT

        [[nodiscard]] id_t id() const;

        template<typename Component>
        [[nodiscard]] bool has() const;

        [[nodiscard]] bool operator==(const EntityBase&) const = default;

        template<typename Component>
        [[nodiscard]] const Component& get() const;

        template<typename Component>
        [[nodiscard]] Component& get() requires(!Const);

        template<typename... Components>
        [[nodiscard]] std::tuple<const Components &...> get_all() const;

        template<typename... Components>
        [[nodiscard]] std::tuple<Components &...> get_all() requires(!Const);

        template<typename C>
        EntityBase& push_back(C&& component);

        template<typename Component, typename... Args>
        EntityBase& emplace(Args&&... args);
    };

    // Implementation ============================================================================

    template<typename C>
    const Storage<C>& Registry::get_storage() const {
        auto id = get_component_id<C>();
        if (id >= storages_.size()) { // TODO: only in debug
            throw std::out_of_range("No storage for component type");
        }
        return static_cast<const Storage<C> &>(*storages_[id]);
    }

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
    void Registry::push_back(const ConstEntity entity, Component&& component) {
        auto& storage = get_storage<std::decay_t<Component> >();
        storage.push_back(entity.id(), std::forward<Component>(component));
    }

    template<typename Component, typename... Args>
    void Registry::emplace(const ConstEntity entity, Args&&... args) {
        auto& storage = get_storage<Component>();
        storage.emplace(entity.id(), std::forward<Args>(args)...);
    }

    inline void Registry::remove(const ConstEntity entity) { // NOLINT
        for (auto&& storage: storages_)
            storage->remove(entity.id());
    }

    template<typename... Cs>
    ImmutableView<Cs...> Registry::view() const {
        return ImmutableView<Cs...>(&get_storage<Cs>()..., this);
    }

    template<typename... Cs>
    MutableView<Cs...> Registry::view() {
        return MutableView<Cs...>(&get_storage<Cs>()..., this);
    }

    inline void Registry::compact_all() { // NOLINT
        for (auto&& storage : storages_)
            storage->compact();
    }

    template<bool Const>
    EntityBase<Const>::EntityBase(const id_t id, Registry* registry): id_(id), registry_(registry) {}

    template<bool Const>
    EntityBase<Const>::operator EntityBase<true>() const {
        return {id_, registry_};
    }

    template<bool Const>
    id_t EntityBase<Const>::id() const {
        return id_;
    }

    template<bool Const>
    template<typename Component>
    bool EntityBase<Const>::has() const {
        return registry_->get_storage<Component>().entity_has(id_);
    }

    template<bool Const>
    template<typename Component>
    const Component& EntityBase<Const>::get() const {
        return registry_->get_storage<Component>().get(id_);
    }

    template<bool Const>
    template<typename Component>
    Component& EntityBase<Const>::get() requires(!Const) {
        return registry_->get_storage<Component>().get(id_);
    }

    template<bool Const>
    template<typename... Components>
    std::tuple<const Components &...> EntityBase<Const>::get_all() const {
        return {get<Components>()...};
    }

    template<bool Const>
    template<typename... Components>
    std::tuple<Components &...> EntityBase<Const>::get_all() requires(!Const) {
        return {get<Components>()...};
    }

    template<bool Const>
    template<typename C>
    EntityBase<Const>& EntityBase<Const>::push_back(C&& component) {
        registry_->push_back(*this, std::forward<C>(component));
        return *this;
    }

    template<bool Const>
    template<typename Component, typename... Args>
    EntityBase<Const>& EntityBase<Const>::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
        return *this;
    }
};
#endif //REGISTRY_H
