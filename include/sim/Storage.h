#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <memory>
#include <numeric>

#include "Types.h"

namespace sim {
    class StorageBase {};

    template<typename T>
    class Storage final : public StorageBase {
        using index_t = uint16_t; // Index type for storage
        static constexpr index_t NO_INDEX = std::numeric_limits<index_t>::max(); // Sentinel value for no index

        std::vector<index_t> id_to_index_; // Sparse
        std::vector<id_t> index_to_id_; // Dense
        std::vector<T> storage_; // Dense

    public:
        [[nodiscard]] size_t size() const;

        [[nodiscard]] bool entity_has(id_t entity_id) const;

        auto&& get(this auto&& self, id_t id);

        void push_back(id_t entity_id, const T& component);

        void push_back(id_t entity_id, T&& component);

        template<typename... Args>
        void emplace(id_t entity_id, Args&&... args);

        void remove(id_t entity_id);

        void for_each(auto&& func);

        struct iterator {
            using value_type = id_t;
            using reference = id_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;
            using iterator_concept = std::forward_iterator_tag;

        private:
            const Storage* storage_;
            index_t index_;

        public:
            iterator(const Storage* storage, index_t index);
            bool operator==(const iterator& other) const = default;
            reference operator*() const;
            iterator& operator++();
        };

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

    private:
        void ensure_mappings(id_t entity_id, index_t index);
    };

    // Implementation ============================================================================

    template<typename T>
    Storage<T>::iterator::iterator(const Storage* storage, const index_t index): storage_(storage), index_(index) {}

    template<typename T>
    typename Storage<T>::iterator::reference Storage<T>::iterator::operator*() const {
        return storage_->index_to_id_[index_];
    }

    template<typename T>
    typename Storage<T>::iterator& Storage<T>::iterator::operator++() {
        ++index_;
        return *this;
    }

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
    auto&& Storage<T>::get(this auto&& self, const id_t id) {
        if (id >= self.id_to_index_.size()) // TODO: only in debug
            throw std::out_of_range("No component for entity with this ID");
        return self.storage_[self.id_to_index_[id]];
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
    void Storage<T>::remove(const id_t entity_id) {
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
            func(id, item);
        }
    }

    template<typename T>
    typename Storage<T>::iterator Storage<T>::begin() const {
        return {this, 0};
    }

    template<typename T>
    typename Storage<T>::iterator Storage<T>::end() const {
        const auto end_index = static_cast<index_t>(storage_.size()); // TODO: some check
        return {this, end_index};
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
