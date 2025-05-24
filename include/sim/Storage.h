#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <memory>
#include <numeric>

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

        [[nodiscard]] bool entity_has(id_t entity_id) const;

        T& entity_get(id_t id);

        void push_back(id_t entity_id, const T& component);

        void push_back(id_t entity_id, T&& component);

        template<typename... Args>
        void emplace(id_t entity_id, Args&&... args);

        void entity_destroy(id_t entity_id);

        void for_each(auto&& func);

    private:
        void ensure_mappings(id_t entity_id, index_t index);
    };

    class EntityHandle;

    // Forward declaration (TODO)
    template<typename... Components>
    class View;

    class Registry {
        std::vector<std::unique_ptr<StorageBase> > storages_;

    public:
        template<typename C>
        [[nodiscard]] Storage<C>& get_storage();

        template<typename Component>
        void push_back(EntityHandle entity, Component&& component);

        template<typename Component, typename... Args>
        void emplace(EntityHandle entity, Args&&... args);

        template<typename... Components>
        [[nodiscard]] View<Components...> view();
    };

    class EntityHandle {
        id_t id_;
        Registry* registry_;

    public:
        EntityHandle(id_t id, Registry* registry);

        [[nodiscard]] id_t id() const;

        template<typename Component>
        [[nodiscard]] bool has() const;

        template<typename Component>
        [[nodiscard]] Component& get() const;

        template<typename Component>
        void push_back(Component&& component);

        template<typename Component, typename... Args>
        void emplace(Args&&... args);
    };

    // Implementation ============================================================================

    template<typename T>
    size_t Storage<T>::size() const {
        return storage_.size();
    }

    template<typename T>
    bool Storage<T>::entity_has(const id_t entity_id) const {
        if (entity_id >= id_to_index_.size())
            return false;
        return id_to_index_[entity_id] != NO_INDEX;
    }

    template<typename T>
    T& Storage<T>::entity_get(const id_t id) {
        if (id >= id_to_index_.size()) // TODO: only in debug
            throw std::out_of_range("ID out of range");
        return storage_[id_to_index_[id]];
    }

    template<typename T>
    void Storage<T>::push_back(const id_t entity_id, const T& component) {
        storage_.push_back(component);
        ensure_mappings(entity_id, storage_.size() - 1);
    }

    template<typename T>
    void Storage<T>::push_back(const id_t entity_id, T&& component) {
        storage_.push_back(std::move(component));
        ensure_mappings(entity_id, storage_.size() - 1);
    }

    template<typename T>
    template<typename... Args>
    void Storage<T>::emplace(const id_t entity_id, Args&&... args) {
        storage_.emplace_back(std::forward<Args>(args)...);
        ensure_mappings(entity_id, storage_.size() - 1);
    }

    template<typename T>
    void Storage<T>::entity_destroy(const id_t entity_id) {
        const index_t index = id_to_index_[entity_id];
        const index_t last_index = storage_.size() - 1;
        const id_t swapped_id = index_to_id_[last_index];
        std::swap(storage_[index], storage_[last_index]);
        std::swap(id_to_index_[entity_id], id_to_index_[swapped_id]);
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

    template<typename T>
    void Storage<T>::ensure_mappings(const id_t entity_id, const index_t index) {
        if (entity_id >= id_to_index_.size())
            id_to_index_.resize(entity_id + 1, NO_INDEX);
        id_to_index_[entity_id] = index;

        if (index >= index_to_id_.size())
            index_to_id_.resize(index + 1, NO_ID);
        index_to_id_[index] = entity_id;
    }

    template<typename Component>
    Storage<Component>& Registry::get_storage() {
        auto id = get_component_id<Component>();
        if (id >= storages_.size()) {
            storages_.resize(id + 1);
            storages_[id] = std::make_unique<Storage<Component> >();
        }
        return static_cast<Storage<Component> &>(*storages_[id]);
    }

    template<typename Component>
    void Registry::push_back(const EntityHandle entity, Component&& component) {
        auto& storage = get_storage<std::decay_t<Component>>();
        storage.push_back(entity.id(), std::forward<Component>(component));
    }

    template<typename Component, typename... Args>
    void Registry::emplace(const EntityHandle entity, Args&&... args) {
        auto& storage = get_storage<Component>();
        storage.emplace(entity.id(), std::forward<Args>(args)...);
    }

    template<typename... Components>
    View<Components...> Registry::view() {
        return View<Components...>(this);
    }

    inline EntityHandle::EntityHandle(const id_t id, Registry* registry): id_(id), registry_(registry) {}

    inline id_t EntityHandle::id() const {
        return id_;
    }

    template<typename Component>
    bool EntityHandle::has() const {
        return registry_->get_storage<Component>().entity_has(id_);
    }

    template<typename Component>
    Component& EntityHandle::get() const {
        return registry_->get_storage<Component>().entity_get(id_);
    }

    template<typename Component>
    void EntityHandle::push_back(Component&& component) {
        registry_->push_back(*this, std::forward<Component>(component));
    }

    template<typename Component, typename... Args>
    void EntityHandle::emplace(Args&&... args) {
        registry_->emplace<Component>(*this, std::forward<Args>(args)...);
    }
}

#endif //STORAGE_H
