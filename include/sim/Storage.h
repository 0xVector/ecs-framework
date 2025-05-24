#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <memory>
#include <numeric>

namespace sim {
    using id_t = uint16_t;
    using index_t = uint16_t;

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
}

#endif //STORAGE_H
