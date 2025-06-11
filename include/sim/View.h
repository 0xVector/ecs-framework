#ifndef VIEW_H
#define VIEW_H

#include "Registry.h"
#include "Traits.h"

namespace sim {
    /// @brief A lightweight view of entities with specific components.
    /// @tparam Imm Whether the view is immutable (true) or mutable (false).
    /// @tparam Cs The component types to include in the view.
    template<bool Imm, typename... Cs>
    class View {
        template<typename C>
        using storage_t = std::conditional_t<Imm, const Storage<C>, Storage<C> >;

        const std::tuple<storage_t<Cs>*...> storages_;
        Registry* registry_;

    public:
        /// @brief Constructs a view with the given storages and registry.
        /// @param storages The storages containing the components of the view.
        /// @param registry A pointer to the registry managing the entities.
        explicit View(storage_t<Cs>*... storages, Registry* registry);

        /// @brief Calls the given callable for each entity in the view.
        /// @details The callable should accept all the components in the view as references to const.
        /// It can also optionally accept the entity itself as the first argument.
        /// @param callable The callable to call for each entity.
        void for_each(auto&& callable) const;

        /// @brief Calls the given callable for each entity in the view.
        /// @details The callable should accept all the components in the view as arguments, and can modify them.
        /// It can also optionally accept the entity itself as the first argument.
        /// @param callable The callable to call for each entity.
        void for_each(auto&& callable) requires (!Imm);

        /// @brief Checks if the view is empty.
        /// @return Whether the view contains any entities.
        [[nodiscard]] bool empty() const;

        template<bool Const>
        struct iterator_base;

        template<bool Const>
        friend struct iterator_base;

        /// @brief A constant iterator for the view.
        using const_iterator = iterator_base<true>;
        /// @brief A mutable iterator for the view.
        using iterator = iterator_base<false>;

        /// @brief Returns a constant iterator to the beginning of the view.
        /// @return A constant iterator to the first entity in the view.
        [[nodiscard]] const_iterator begin() const;

        /// @brief Returns a constant iterator to the end of the view.
        /// @return A constant iterator to the end of the view.
        [[nodiscard]] const_iterator end() const;

        /// @brief Returns a mutable iterator to the beginning of the view.
        /// @return A mutable iterator to the first entity in the view.
        [[nodiscard]] iterator begin() requires (!Imm);

        /// @brief Returns a mutable iterator to the end of the view.
        /// @return A mutable iterator to the end of the view.
        [[nodiscard]] iterator end() requires (!Imm);
    };

    /// @brief View iterator base class.
    /// @tparam Imm Whether the view is immutable (true) or mutable (false).
    /// @tparam Cs The component types in the view.
    template<bool Imm, typename... Cs>
    template<bool Const>
    struct View<Imm, Cs...>::iterator_base {
    private:
        using entity_t = std::conditional_t<Const, ConstEntity, Entity>;

    public:
        using value_type = entity_t;
        using reference = entity_t;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::input_iterator_tag;

    private:
        using view_t = std::conditional_t<Const, const View, View>;
        using it_t = typename Storage<first_t<Cs...> >::iterator;

        view_t* view_;
        it_t it_;

    public:
        /// @brief The default constructor creates an invalid iterator.
        iterator_base() = default;

        /// @brief Constructs an iterator for the given view and storage iterator.
        explicit iterator_base(view_t* view, typename Storage<first_t<Cs...> >::iterator it);

        /// @brief Checks if this iterator is equal to another iterator.
        /// @param other The other iterator to compare with.
        /// @return Whether the two iterators are equal.
        bool operator==(const iterator_base& other) const;

        /// @brief Dereferences the iterator to get the current entity.
        /// @return A reference to the current entity.
        [[nodiscard]] reference operator*() const;

        /// @brief Pre-increments the iterator to the next entity.
        /// @return A reference to this iterator after incrementing.
        iterator_base& operator++();

        /// @brief Post-increments the iterator to the next entity.
        /// @return A copy of this iterator before incrementing.
        iterator_base operator++(int);

    private:
        // Advances the iterator until it points to a valid entity.
        void advance_till_valid();
    };

    /// @brief Provides the context of the current simulation cycle and allows access to entities and views.
    class Context {
        const size_t cycle_ = 0;
        Registry* registry_;

    public:
        /// @brief Constructs a context for the given registry and cycle.
        explicit Context(Registry* registry, size_t cycle);

        Context(const Context&) = default;
        Context(Context&&) = default;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;

        /// @brief Returns the current simulation cycle.
        /// @return The current cycle number.
        [[nodiscard]] size_t cycle() const;

        /// @brief Returns an immutable view of entities with the specified components.
        /// @tparam Cs The component types to include in the view.
        /// @return An immutable view of entities with the specified components.
        template<typename... Cs>
        [[nodiscard]] ImmutableView<Cs...> view() const;

        /// @brief Returns a mutable view of entities with the specified components.
        /// @tparam Cs The component types to include in the view.
        /// @return A mutable view of entities with the specified components.
        template<typename... Cs>
        [[nodiscard]] MutableView<Cs...> view();

        /// @brief Gets an immutable Entity handle by its ID.
        /// @param entity_id The ID of the entity to retrieve.
        /// @return A ConstEntity with the specified ID.
        [[nodiscard]] ConstEntity get_entity(id_t entity_id) const;

        /// @brief Gets a mutable Entity handle by its ID.
        /// @param entity_id The ID of the entity to retrieve.
        /// @return An Entity with the specified ID.
        [[nodiscard]] Entity get_entity(id_t entity_id);

        /// @brief Removes an entity from the registry.
        void remove_entity(ConstEntity entity);

        /// @brief Removes an entity from the registry by its ID.
        void remove_entity(id_t entity_id);
    };

    // Implementation ============================================================================

    template<bool Imm, typename... Cs>
    View<Imm, Cs...>::View(storage_t<Cs>*... storages, Registry* registry):
        storages_(storages...), registry_(registry) {}

    template<bool Imm, typename... Cs>
    void View<Imm, Cs...>::for_each(auto&& callable) const {
        for (ConstEntity entity: *this) {
            if constexpr (requires { std::forward<decltype(callable)>(callable)(entity, entity.get<Cs>()...); })
                std::forward<decltype(callable)>(callable)(entity, entity.get<Cs>()...);
            else
                std::apply(std::forward<decltype(callable)>(callable), entity.get_all<Cs...>());
        }
    }

    template<bool Imm, typename... Cs>
    void View<Imm, Cs...>::for_each(auto&& callable) requires (!Imm) {
        for (Entity entity: *this) {
            if constexpr (requires { callable(entity, entity.get<Cs>()...); })
                std::forward<decltype(callable)>(callable)(entity, entity.get<Cs>()...);
            else
                std::apply(std::forward<decltype(callable)>(callable), entity.get_all<Cs...>());
        }
    }

    template<bool Imm, typename... Cs>
    bool View<Imm, Cs...>::empty() const {
        return begin() == end();
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    View<Imm, Cs...>::iterator_base<Const>::iterator_base(view_t* view, typename Storage<first_t<Cs...> >::iterator it):
        view_(view), it_(it) {
        advance_till_valid();
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    bool View<Imm, Cs...>::iterator_base<Const>::operator==(const iterator_base& other) const {
        return view_ == other.view_ && it_ == other.it_;
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    typename View<Imm, Cs...>::template iterator_base<Const>::reference View<Imm, Cs...>::iterator_base<Const>
    ::operator*() const {
        return Entity(*it_, view_->registry_);
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    typename View<Imm, Cs...>::template iterator_base<Const>& View<Imm, Cs...>::iterator_base<Const>::operator++() {
        ++it_;
        advance_till_valid();
        return *this;
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    typename View<Imm, Cs...>::template iterator_base<Const> View<Imm, Cs...>::iterator_base<Const>::operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    template<bool Imm, typename... Cs>
    template<bool Const>
    void View<Imm, Cs...>::iterator_base<Const>::advance_till_valid() {
        const auto it_end = std::get<0>(view_->storages_)->end();
        while (it_ != it_end) {
            if ((... && std::get<Storage<Cs>*>(view_->storages_)->entity_has(*it_)))
                break;
            ++it_;
        }
    }

    template<bool Imm, typename... Cs>
    typename View<Imm, Cs...>::const_iterator View<Imm, Cs...>::begin() const {
        return const_iterator(this, std::get<0>(storages_)->begin());
    }

    template<bool Imm, typename... Cs>
    typename View<Imm, Cs...>::const_iterator View<Imm, Cs...>::end() const {
        return const_iterator(this, std::get<0>(storages_)->end());
    }

    template<bool Imm, typename... Cs>
    typename View<Imm, Cs...>::iterator View<Imm, Cs...>::begin() requires (!Imm) {
        return iterator(this, std::get<0>(storages_)->begin());
    }

    template<bool Imm, typename... Cs>
    typename View<Imm, Cs...>::iterator View<Imm, Cs...>::end() requires (!Imm) {
        return iterator(this, std::get<0>(storages_)->end());
    }

    template<typename... Cs>
    ImmutableView<Cs...> Context::view() const {
        return registry_->view<Cs...>();
    }

    template<typename... Cs>
    MutableView<Cs...> Context::view() {
        return registry_->view<Cs...>();
    }

    inline Context::Context(Registry* registry, const size_t cycle): cycle_(cycle), registry_(registry) {}

    inline size_t Context::cycle() const {
        return cycle_;
    }

    inline ConstEntity Context::get_entity(id_t entity_id) const {
        return {entity_id, registry_};
    }

    inline Entity Context::get_entity(id_t entity_id) {
        return {entity_id, registry_};
    }

    inline void Context::remove_entity(const ConstEntity entity) { // NOLINT
        registry_->remove(entity);
    }

    inline void Context::remove_entity(const id_t entity_id) {
        remove_entity(get_entity(entity_id));
    }
}
#endif //VIEW_H
