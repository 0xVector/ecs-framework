#ifndef VIEW_H
#define VIEW_H

#include "Registry.h"
#include "Traits.h"

namespace sim {
    template<typename... Cs>
    class View {
        const std::tuple<Storage<Cs>*...> storages_;
        Registry* registry_;

    public:
        friend struct iterator;

        struct iterator {
            using value_type = Entity;
            using reference = Entity;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;
            using iterator_concept = std::forward_iterator_tag;

        private:
            View* view_;
            typename Storage<first_t<Cs...> >::iterator it_;

        public:
            explicit iterator(View* view, typename Storage<first_t<Cs...> >::iterator it);
            bool operator==(const iterator& other) const = default;
            reference operator*() const;
            iterator& operator++();

        private:
            void advance_till_valid();
        };

        explicit View(Storage<Cs>*... storages, Registry* registry);
        void for_each(auto&& func);
        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
    };

    class Context {
        Registry* registry_;

    public:
        explicit Context(Registry* registry);

        template<typename... Cs>
        [[nodiscard]] View<Cs...> view();
    };

    // Implementation ============================================================================

    template<typename... Cs>
    View<Cs...>::iterator::iterator(View* view, typename Storage<first_t<Cs...> >::iterator it):
        view_(view), it_(it) {
        advance_till_valid();
    }

    template<typename... Cs>
    typename View<Cs...>::iterator::reference View<Cs...>::iterator::operator*() const {
        return Entity(*it_, view_->registry_);
    }

    template<typename... Cs>
    typename View<Cs...>::iterator& View<Cs...>::iterator::operator++() {
        ++it_;
        advance_till_valid();
        return *this;
    }

    template<typename... Cs>
    void View<Cs...>::iterator::advance_till_valid() {
        const auto it_end = std::get<0>(view_->storages_)->end();
        while (it_ != it_end) {
            if ((... && std::get<Storage<Cs>*>(view_->storages_)->entity_has(*it_)))
                break;
            ++it_;
        }
    }

    template<typename... Cs>
    View<Cs...>::View(Storage<Cs>*... storages, Registry* registry): storages_(storages...), registry_(registry) {}

    template<typename... Cs>
    void View<Cs...>::for_each(auto&& func) {
        for (Entity entity: *this) {
            if constexpr (requires { std::forward<decltype(func)>(func)(entity, entity.get<Cs>()...); })
                std::forward<decltype(func)>(func)(entity, entity.get<Cs>()...);
            else
                std::apply(std::forward<decltype(func)>(func), entity.get_all<Cs...>());
        }
    }

    template<typename... Cs>
    typename View<Cs...>::iterator View<Cs...>::begin() {
        return iterator(this, std::get<0>(storages_)->begin());
    }

    template<typename... Cs>
    typename View<Cs...>::iterator View<Cs...>::end() {
        return iterator(this, std::get<0>(storages_)->end());
    }

    inline Context::Context(Registry* registry): registry_(registry) {}

    template<typename... Cs>
    View<Cs...> Context::view() {
        return registry_->view<Cs...>();
    }
}
#endif //VIEW_H
