#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <memory>

namespace sim {
    using id_t = uint16_t;
    using index_t = uint16_t;
    using component_id_t = size_t;

    template<typename... Cs>
    class Components {};

    inline component_id_t generate_component_id() {
        static component_id_t id = 0;
        return id++;
    }

    template<typename C>
    component_id_t get_component_id() {
        static component_id_t id = generate_component_id();
        return id;
    }

    class StorageBase {};

    template<typename T>
    class Storage final : public StorageBase {
        static constexpr index_t NO_INDEX = std::numeric_limits<index_t>::max();
        static constexpr id_t NO_ID = std::numeric_limits<id_t>::max();
        std::vector<index_t> id_to_index_;
        std::vector<id_t> index_to_id_;
        std::vector<T> storage_;

    public:
        [[nodiscard]] size_t size() const;

        [[nodiscard]] bool has(id_t id) const;

        T& get(id_t id);

        template<typename... Args>
        void emplace(id_t id, Args&&... args);

        void destroy(id_t id);

        void for_each(auto&& func);
    };

    class EntityHandle;

    // Forward declaration (TODO)
    template<typename... Components>
    class View;

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename C>
        Storage<C>& get();

        template<typename Component, typename... Args>
        void emplace(EntityHandle entity, Args&&... args);

        template<typename ... Components>
        View<Components...> view();
    };

    class EntityHandle {
        id_t id_;
        Registry* registry_;

    public:
        EntityHandle(id_t index, Registry* registry);

        [[nodiscard]] id_t id() const;

        template<typename Component>
        [[nodiscard]] bool has() const;

        template<typename Component>
        [[nodiscard]] Component& get() const;

        template<typename Component, typename... Args>
        void emplace(Args&&... args);
    };

    // Implementation ============================================================================

    template<typename T>
    size_t Storage<T>::size() const {
        return storage_.size();
    }

    template<typename T>
    bool Storage<T>::has(const id_t id) const {
        if (id >= id_to_index_.size())
            return false;
        return id_to_index_[id] != NO_INDEX;
    }

    template<typename T>
    T& Storage<T>::get(const id_t id) {
        if (id >= id_to_index_.size()) // TODO: only in debug
            throw std::out_of_range("ID out of range");
        return storage_[id_to_index_[id]];
    }

    template<typename T>
    template<typename... Args>
    void Storage<T>::emplace(const id_t id, Args&&... args) {
        storage_.emplace_back(std::forward<Args>(args)...);
        const index_t index = storage_.size() - 1;

        if (id >= id_to_index_.size())
            id_to_index_.resize(id + 1, NO_INDEX);
        id_to_index_[id] = index;

        if (index >= index_to_id_.size())
            index_to_id_.resize(index + 1, NO_ID);
        index_to_id_[index] = id;
    }

    template<typename T>
    void Storage<T>::destroy(const id_t id) {
        const index_t index = id_to_index_[id];
        const index_t last_index = storage_.size() - 1;
        const id_t swapped_id = index_to_id_[last_index];
        std::swap(storage_[index], storage_[last_index]);
        std::swap(id_to_index_[id], id_to_index_[swapped_id]);
        std::swap(index_to_id_[index], index_to_id_[last_index]);
        storage_.pop_back();
    }

    template<typename T>
    void Storage<T>::for_each(auto&& func) {
        for (size_t i = 0; i < storage_.size(); ++i) {
            const id_t id = index_to_id_[i];
            T& item = storage_[i];
            if constexpr (requires { func(id, item); })
                func(id, item);
            else
                func(item);
        }
    }

    template<typename Component>
    Storage<Component>& Registry::get() {
        auto id = get_component_id<Component>();
        if (id >= storages_.size()) {
            storages_.resize(id + 1);
            storages_[id] = std::make_unique<Storage<Component> >();
        }
        return static_cast<Storage<Component> &>(*storages_[id]);
    }

    template<typename Component, typename... Args>
    void Registry::emplace(const EntityHandle entity, Args&&... args) {
        Storage<Component>& storage = get<Component>();
        storage.emplace(entity.id(), std::forward<Args>(args)...);
    }

    template<typename ... Components>
    View<Components...> Registry::view() {
        return View<Components...>(this);
    }

    inline EntityHandle::EntityHandle(const id_t index, Registry* registry): id_(index), registry_(registry) {}

    inline id_t EntityHandle::id() const {
        return id_;
    }

    template<typename Component>
    bool EntityHandle::has() const {
        return registry_->get<Component>();
    }

    template<typename Component>
    Component& EntityHandle::get() const {
        return registry_->get<Component>();
    }

    template<typename Component, typename... Args>
    void EntityHandle::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
    }
}

#endif //STORAGE_H
