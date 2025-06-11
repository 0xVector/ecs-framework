#ifndef VIEW_H
#define VIEW_H

#include "Registry.h"
#include "Traits.h"

namespace sim {
    template<bool Imm, typename... Cs>
    class View {
        template<typename C>
        using storage_t = std::conditional_t<Imm, const Storage<C>, Storage<C> >;

        const std::tuple<storage_t<Cs>*...> storages_;
        Registry* registry_;

    public:
        explicit View(storage_t<Cs>*... storages, Registry* registry);

        void for_each(auto&& func) const;

        void for_each(auto&& func) requires (!Imm);

        [[nodiscard]] bool empty() const;

        template<bool Const>
        struct iterator_base;

        template<bool Const>
        friend struct iterator_base;

        using const_iterator = iterator_base<true>;
        using iterator = iterator_base<false>;

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] iterator begin() requires (!Imm);
        [[nodiscard]] iterator end() requires (!Imm);
    };

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
        iterator_base() = default;
        explicit iterator_base(view_t* view, typename Storage<first_t<Cs...> >::iterator it);

        bool operator==(const iterator_base& other) const;
        reference operator*() const;
        iterator_base& operator++();
        iterator_base operator++(int);

    private:
        void advance_till_valid();
    };

    class Context {
        const size_t cycle_ = 0;
        Registry* registry_;

    public:
        explicit Context(Registry* registry, size_t cycle);

        Context(const Context&) = default;
        Context(Context&&) = default;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;

        [[nodiscard]] size_t cycle() const;

        template<typename... Cs>
        [[nodiscard]] ImmutableView<Cs...> view() const;

        template<typename... Cs>
        [[nodiscard]] MutableView<Cs...> view();

        [[nodiscard]] ConstEntity get_entity(id_t entity_id) const;

        [[nodiscard]] Entity get_entity(id_t entity_id);

        void remove_entity(ConstEntity entity);
        void remove_entity(id_t entity_id);
    };

    // Implementation ============================================================================

    template<bool Imm, typename... Cs>
    View<Imm, Cs...>::View(storage_t<Cs>*... storages, Registry* registry):
        storages_(storages...), registry_(registry) {}

    template<bool Imm, typename... Cs>
    void View<Imm, Cs...>::for_each(auto&& func) const {
        for (ConstEntity entity: *this) {
            if constexpr (requires { std::forward<decltype(func)>(func)(entity, entity.get<Cs>()...); })
                std::forward<decltype(func)>(func)(entity, entity.get<Cs>()...);
            else
                std::apply(std::forward<decltype(func)>(func), entity.get_all<Cs...>());
        }
    }

    template<bool Imm, typename... Cs>
    void View<Imm, Cs...>::for_each(auto&& func) requires (!Imm) {
        for (Entity entity: *this) {
            if constexpr (requires { func(entity, entity.get<Cs>()...); })
                std::forward<decltype(func)>(func)(entity, entity.get<Cs>()...);
            else
                std::apply(std::forward<decltype(func)>(func), entity.get_all<Cs...>());
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
