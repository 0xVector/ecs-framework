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
        std::vector<index_t> id_to_index_;
        std::vector<id_t> index_to_id_;
        std::vector<T> storage_;

    public:
        [[nodiscard]] size_t size() const;

        T& get(id_t id);

        template<typename... Args>
        void emplace(id_t id, Args&&... args);

        void destroy(id_t id);

        void for_each(auto&& func);
    };

    class EntityHandle;

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename C>
        Storage<C>& get();

        template<typename Component, typename... Args>
        void emplace(EntityHandle entity, Args&&... args);
    };

    class EntityHandle {
        id_t id_;
        Registry* registry_;

    public:
        EntityHandle(id_t index, Registry* registry);

        [[nodiscard]] id_t id() const;

        template<typename Component, typename... Args>
        void emplace(Args&&... args);
    };

    // Implementation ============================================================================

    template<typename T>
    size_t Storage<T>::size() const {
        return storage_.size();
    }

    template<typename T>
    T& Storage<T>::get(const id_t id) {
        return storage_[id_to_index_[id]];
    }

    template<typename T>
    template<typename... Args>
    void Storage<T>::emplace(const id_t id, Args&&... args) {
        storage_.emplace_back(std::forward<Args>(args)...);
        const index_t index = storage_.size() - 1;

        if (id >= id_to_index_.size())
            id_to_index_.resize(id + 1);
        id_to_index_[id] = index;

        if (index >= index_to_id_.size())
            index_to_id_.resize(index + 1);
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
        for (auto&& it: storage_) {
            func(it);
            // TODO
            // if constexpr ( requires {func(it);}) {
            // }
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

    inline EntityHandle::EntityHandle(const id_t index, Registry* registry): id_(index), registry_(registry) {}

    inline id_t EntityHandle::id() const {
        return id_;
    }

    template<typename Component, typename... Args>
    void EntityHandle::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
    }
}

#endif //STORAGE_H
