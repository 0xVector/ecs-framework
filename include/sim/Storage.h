#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <memory>

namespace sim {
    using index_t = uint16_t;
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

    template<typename... Cs>
    class ComponentsPack {};

    class StorageBase {};

    template<typename T>
    class Storage final : public StorageBase {
        std::vector<index_t> mapping_;
        std::vector<T> storage_;

    public:
        [[nodiscard]] size_t size() const;

        template<typename... Args>
        void emplace(index_t index, Args&&... args);

        void for_each(auto&& func);
    };

    class EntityHandle;

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename Component>
        Storage<Component>& get();

        template<typename Component, typename... Args>
        void emplace(EntityHandle entity, Args&&... args);
    };

    class EntityHandle {
        index_t index_;
        Registry* registry_;

    public:
        EntityHandle(index_t index, Registry* registry);

        [[nodiscard]] index_t index() const;

        template<typename Component, typename... Args>
        void emplace(Args&&... args);
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
        auto& storage = get<Component>();
        storage.emplace(entity.index(), std::forward<Args>(args)...);
    }

    inline EntityHandle::EntityHandle(const index_t index, Registry* registry): index_(index), registry_(registry) {}

    inline index_t EntityHandle::index() const {
        return index_;
    }

    template<typename Component, typename... Args>
    void EntityHandle::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
    }
}

#endif //STORAGE_H
