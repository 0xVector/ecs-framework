#ifndef STORAGE_H
#define STORAGE_H
#include <algorithm>
#include <vector>
#include <memory>
#include <numeric>

#include "Types.h"

namespace sim {
    class StorageBase {
    public:
        virtual ~StorageBase() = default;
        virtual void remove(id_t entity_id) = 0;
        virtual void compact() = 0;
    };

    template<typename T>
    class Storage final : public StorageBase {
        using index_t = uint16_t; // Index type for storage
        static constexpr index_t NO_INDEX = std::numeric_limits<index_t>::max(); // Sentinel value for no index

        std::vector<index_t> id_to_index_; // Sparse
        std::vector<id_t> index_to_id_; // Dense
        std::vector<T> storage_; // Dense

    public:
        using iterator = std::vector<id_t>::const_iterator;

        explicit Storage() = default;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] bool entity_has(id_t entity_id) const;

        auto&& get(this auto&& self, id_t id);

        void push_back(id_t entity_id, const T& component);

        void push_back(id_t entity_id, T&& component);

        template<typename... Args>
        void emplace(id_t entity_id, Args&&... args);

        void remove(id_t entity_id) override;

        void remove_unsafe(id_t entity_id);

        void for_each(auto&& func);

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

        void compact() override;

    private:
        void ensure_mappings(id_t entity_id, index_t index);
        void swap_remove_at(index_t index);
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

    // Remove the entity from the storage, but do not compact it
    // This doesn't completely the component, but marks it as removed
    // Doesn't invalidate iterators or references to the storage elements
    template<typename T>
    void Storage<T>::remove(const id_t entity_id) {
        if (!entity_has(entity_id)) return; // TODO: check instead?
        const index_t index = id_to_index_[entity_id];
        id_to_index_[entity_id] = NO_INDEX;
        index_to_id_[index] = NO_ID; // Mark the index as unused
    }

    // Remove and compact the storage
    template<typename T>
    void Storage<T>::remove_unsafe(const id_t entity_id) {
        const index_t index = id_to_index_[entity_id];
        const index_t last_index = storage_.size() - 1;
        const id_t swapped_id = index_to_id_[last_index];
        std::swap(storage_[index], storage_[last_index]);
        std::swap(id_to_index_[entity_id], id_to_index_[swapped_id]);
        std::swap(index_to_id_[index], index_to_id_[last_index]);
        storage_.pop_back();
        index_to_id_.pop_back();
    }

    template<typename T>
    void Storage<T>::for_each(auto&& func) { // TODO: through ranges natively
        for (size_t i = 0; i < storage_.size(); ++i) {
            const id_t id = index_to_id_[i];
            T& item = storage_[i];
            func(id, item);
        }
    }

    template<typename T>
    typename Storage<T>::iterator Storage<T>::begin() const {
        return index_to_id_.begin();
    }

    template<typename T>
    typename Storage<T>::iterator Storage<T>::end() const {
        return index_to_id_.end();
    }

    // Compact the storage by removing unused indices and shifting elements
    // This invalidates all iterators and references to the storage elements
    // It can still leave some unused indices after one sweep, if there were empty mappings at the end (TODO)
    template<typename T>
    void Storage<T>::compact() {
        for (index_t i = 0; i < index_to_id_.size(); ++i)
            if (index_to_id_[i] == NO_ID)
                swap_remove_at(i);
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

    // Remove an index from the storage by swapping it with the last element
    // The index will be overwritten, and the last element will be moved to the index
    template<typename T>
    void Storage<T>::swap_remove_at(index_t index) {
        if (index_to_id_[index] != NO_ID)
            throw std::runtime_error("Cannot remove an index that is not marked as unused");

        const index_t last_index = storage_.size() - 1;
        const id_t last_id = index_to_id_[last_index];

        std::swap(storage_[index], storage_[last_index]);
        std::swap(index_to_id_[index], index_to_id_[last_index]);
        if (last_id != NO_ID)
            id_to_index_[last_id] = index; // Update the id_to_index_ mapping

        storage_.pop_back();
        index_to_id_.pop_back();
    }
}

#endif //STORAGE_H
