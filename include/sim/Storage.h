#ifndef STORAGE_H
#define STORAGE_H
#include <vector>

namespace sim {
    using index_t = uint16_t;

    template<typename... Cs>
    class EntityHandle;

    template<typename T>
    class Storage {
        std::vector<index_t> mapping_;
        std::vector<T> storage_;

    public:
        [[nodiscard]] size_t size() const;

        template<typename... Args>
        void emplace(index_t index, Args&&... args);

        void for_each(auto&& func);
    };

    template<typename... Cs>
    class Registry {
        using handle_t = EntityHandle<Cs...>;
        std::tuple<Storage<Cs>...> storages_;

    public:
        template<typename Component, typename... Args>
            requires OneOf<Component, Cs...>
        void emplace(handle_t entity, Args&&... args);

        template<typename Component>
            requires OneOf<Component, Cs...>
        [[nodiscard]] Storage<Component>& get_storage();

        template<typename Event>
        void dispatch(const Event& event);
    };

    template<typename... Cs>
    class EntityHandle {
        using storage_t = Registry<Cs...>;
        index_t index_;
        storage_t* storage_;

    public:
        EntityHandle(index_t index, storage_t* storage);

        [[nodiscard]] index_t index() const;

        template<typename Component, typename... Args>
            requires OneOf<Component, Cs...>
        void emplace(Args&&... args);
    };

    template<typename A, typename B>
    class View {
        Storage<A>* storage_a_;
        Storage<B>* storage_b_;

    public:
        View(Storage<A>* storage_a, Storage<B>* storage_b);
        void for_each(auto&& func);
    };

    template<typename... Cs>
    class Context {
        using registry_t = Registry<Cs...>;
        registry_t* registry_;

    public:
        explicit Context(registry_t* registry);

        template<typename A, typename B>
        [[nodiscard]] View<A, B> view();
    };

    // Implementation ============================================================================

    template<typename T>
    size_t Storage<T>::size() const {
        return storage_.size();
    }

    template<typename T>
    template<typename... Args>
    void Storage<T>::emplace(const index_t index, Args&&... args) {
        storage_.emplace_back(std::forward<Args>(args)...);

        if (index >= mapping_.size())
            mapping_.resize(index + 1);
        mapping_[index] = storage_.size();
    }

    template<typename T>
    void Storage<T>::for_each(auto&& func) {
        for (auto&& it: storage_)
            func(it);
    }

    template<typename... Cs>
    template<typename Component, typename... Args>
        requires OneOf<Component, Cs...>
    void Registry<Cs...>::emplace(handle_t entity, Args&&... args) {
        auto& storage = std::get<Storage<Component> >(storages_);
        storage.emplace(entity.index(), std::forward<Args>(args)...);
    }

    template<typename ... Cs>
    template<typename Component> requires OneOf<Component, Cs...>
    Storage<Component>& Registry<Cs...>::get_storage() {
        return std::get<Storage<Component> >(storages_);
    }

    template<typename... Cs>
    template<typename Event>
    void Registry<Cs...>::dispatch(const Event& event) {
        Context<Cs...> context(this);

        std::apply([&](Storage<Cs>&... storage) {
            (storage.for_each([&](auto&& component) {
                // if constexpr (requires { component(*this, event, context); }) {
                //     component(*this, event, context);
                // }
                // if constexpr (requires { component(*this, event); }) {
                //     component(*this, event);
                // }
                if constexpr (requires { component(event, context); }) {
                    component(event, context);
                }
                if constexpr (requires { component(event); }) {
                    component(event);
                }
            }), ...);
        }, storages_);
    }

    template<typename... Cs>
    EntityHandle<Cs...>::EntityHandle(const index_t index, storage_t* storage): index_(index), storage_(storage) {}

    template<typename... Cs>
    index_t EntityHandle<Cs...>::index() const { return index_; }

    template<typename... Cs>
    template<typename Component, typename... Args>
        requires OneOf<Component, Cs...>
    void EntityHandle<Cs...>::emplace(Args&&... args) {
        storage_->template emplace<Component>(*this, std::forward<Args>(args)...);
    }

    template<typename A, typename B>
    View<A, B>::View(Storage<A>* storage_a, Storage<B>* storage_b): storage_a_(storage_a), storage_b_(storage_b) {}

    template<typename A, typename B>
    void View<A, B>::for_each(auto&& func) {
        storage_a_->for_each([&](auto&& a) {
            storage_b_->for_each([&](auto&& b) {
                func(a, b);
            });
        });
    }

    template<typename ... Cs>
    Context<Cs...>::Context(registry_t* registry): registry_(registry) {}

    template<typename ... Cs>
    template<typename A, typename B>
    View<A, B> Context<Cs...>::view() {
        return {&registry_->template get_storage<A>(), &registry_->template get_storage<B>()};
    }
}

#endif //STORAGE_H
