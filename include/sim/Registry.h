#ifndef REGISTRY_H
#define REGISTRY_H
#include "Storage.h"

namespace sim {
    /// @brief Unique identifier for a component type.
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

    /// @brief A mutable entity handle.
    using Entity = EntityBase<false>;

    /// @brief An immutable entity handle.
    using ConstEntity = EntityBase<true>;

    // Forward declaration (TODO)
    template<bool Imm, typename... Cs>
    class View;

    /// @brief An immutable view over a set of components.
    /// @tparam Cs Component types to include in the view.
    template<typename... Cs>
    using ImmutableView = View<true, Cs...>;

    /// @brief A mutable view over a set of components.
    /// @tparam Cs Component types to include in the view.
    template<typename... Cs>
    using MutableView = View<false, Cs...>;

    // ======================================================================================================

    /// @brief A registry that manages entities and their components.
    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        /// @brief Gets the storage for a specific component type.
        /// @tparam C The component type.
        /// @return A const reference to the storage for the component type.
        template<typename C>
        [[nodiscard]] const Storage<C>& get_storage() const;

        /// @brief Gets the storage for a specific component type.
        /// @tparam C The component type.
        /// @return A mutable reference to the storage for the component type.
        template<typename C>
        [[nodiscard]] Storage<C>& get_storage();

        /// @brief Pushes a component to an entity.
        /// @tparam C The component type.
        /// @param entity The entity to which the component is added.
        /// @param component The component to add.
        template<typename C>
        void push_back(ConstEntity entity, C&& component);

        /// @brief Emplaces a component to an entity.
        /// @tparam Component The component type.
        /// @tparam Args The types of arguments to construct the component.
        /// @param entity The entity to which the component is added.
        /// @param args The arguments to construct the component.
        template<typename Component, typename... Args>
        void emplace(ConstEntity entity, Args&&... args);

        /// @brief Removes all components from an entity.
        /// @param entity The entity from which components are removed.
        void remove(ConstEntity entity);

        /// @brief Creates an immutable view over a set of components.
        /// @tparam Cs The component types to include in the view.
        /// @return An immutable view over the specified component types.
        template<typename... Cs>
        [[nodiscard]] ImmutableView<Cs...> view() const;

        /// @brief Creates a mutable view over a set of components.
        /// @tparam Cs The component types to include in the view.
        /// @return A mutable view over the specified component types.
        template<typename... Cs>
        [[nodiscard]] MutableView<Cs...> view();

        /// @brief Compacts all storages, removing gaps in the entity IDs. Invalidates all iterators and references.
        void compact_all();
    };

    /// @brief The base class for entity handles, providing access to the entity's ID and its components.
    /// @tparam Const If true, the entity handle is immutable; otherwise, it is mutable.
    template<bool Const>
    class [[nodiscard]] EntityBase {
        id_t id_;
        Registry* registry_;

    public:
        /// @brief Constructs an EntityBase with a specific ID and registry.
        EntityBase(id_t id, Registry* registry);

        /// @brief Converts this mutable entity handle to an immutable entity handle.
        [[nodiscard]] operator EntityBase<true>() const; // NOLINT

        /// @brief Gets the ID of the entity.
        /// @return The ID of the entity.
        [[nodiscard]] id_t id() const;

        /// @brief Checks if the entity has a specific component.
        /// @tparam Component The component type to check for.
        /// @return True if the entity has the component, false otherwise.
        template<typename Component>
        [[nodiscard]] bool has() const;

        /// @brief Checks if two entities are equal.
        /// @return Whether the two entities are equal.
        [[nodiscard]] bool operator==(const EntityBase&) const = default;

        /// @brief Gets a specific component from the entity.
        /// @tparam Component The component type to get.
        /// @return A const reference to the component.
        template<typename Component>
        [[nodiscard]] const Component& get() const;

        /// @brief Gets a specific component from the entity.
        /// @tparam Component The component type to get.
        /// @return A mutable reference to the component.
        template<typename Component>
        [[nodiscard]] Component& get() requires(!Const);

        /// @brief Gets all components of specified types from the entity. Useful for structured bindings.
        /// @tparam Components The component types to get.
        /// @return A tuple containing const references to the components.
        template<typename... Components>
        [[nodiscard]] std::tuple<const Components &...> get_all() const;

        /// @brief Gets all components of specified types from the entity. Useful for structured bindings.
        /// @tparam Components The component types to get.
        /// @return A tuple containing mutable references to the components.
        template<typename... Components>
        [[nodiscard]] std::tuple<Components &...> get_all() requires(!Const);

        /// @brief Pushes a component to the entity.
        /// @tparam C The component type.
        /// @param component The component to add.
        /// @return A reference to this entity handle, allowing for method chaining.
        template<typename C>
        EntityBase& push_back(C&& component);

        /// @brief Emplaces a component to the entity.
        /// @tparam Component The component type.
        /// @tparam Args The types of arguments to construct the component.
        /// @param args The arguments to construct the component.
        /// @return A reference to this entity handle, allowing for method chaining.
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
