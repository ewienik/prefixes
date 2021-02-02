#ifndef MEM_HPP
#define MEM_HPP

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <list>
#include <vector>

///
template <typename T, std::size_t N>
class Mem final {
    using AlignedStorage = std::aligned_storage_t<sizeof(T), alignof(T)>;
    using ArrayStorage = std::array<char, N * sizeof(AlignedStorage)>;
    std::list<ArrayStorage> storage_{};
    std::vector<std::bitset<N>> allocated_{};
    int capacity_{};

public:
    ~Mem() = default;
    Mem() = default;
    Mem(Mem const &) = delete;
    Mem(Mem &&) = delete;

    void operator=(Mem const &) = delete;
    void operator=(Mem &&) = delete;

    ///
    auto Malloc() -> void * {
        if (capacity_ == 0) {
            alignas(T) ArrayStorage arr{};
            storage_.emplace_back(std::move(arr));
            allocated_.emplace_back();
            capacity_ = N;
        }
        auto slot_a =
            std::find_if(std::begin(allocated_), std::end(allocated_), [](auto const &alloc) { return !alloc.all(); });
        assert(slot_a != std::end(allocated_));
        auto block_a = [slot_a]() {
            for (auto it = 0; it < slot_a->size(); ++it) {
                if (!(*slot_a)[it]) { return it; }
            }
            return -1;
        }();
        assert(block_a >= 0);
        assert(storage_.size() == allocated_.size());
        auto slot_s = std::begin(storage_);
        std::advance(slot_s, std::distance(std::begin(allocated_), slot_a));
        auto block_s = std::begin(*slot_s);
        std::advance(block_s, block_a * sizeof(AlignedStorage));
        (*slot_a)[block_a] = true;
        --capacity_;
        return &(*block_s);
    }

    ///
    void Free(void *ptr) {
        assert(storage_.size() == allocated_.size());
        auto slot_s = std::find_if(std::begin(storage_), std::end(storage_), [&ptr](auto const &slot) {
            auto distance = static_cast<char *>(ptr) - slot.data();
            return distance >= 0 && distance < N * sizeof(AlignedStorage);
        });
        assert(slot_s != std::end(storage_));
        auto distance = static_cast<char *>(ptr) - slot_s->data();
        assert(distance >= 0 && distance < N * sizeof(AlignedStorage) && distance % sizeof(AlignedStorage) == 0);
        auto slot_a = std::begin(allocated_);
        std::advance(slot_a, std::distance(std::begin(storage_), slot_s));
        auto block_a = distance / sizeof(AlignedStorage);
        (*slot_a)[block_a] = false;
        ++capacity_;
    }
};

/**/

#endif  // #ifndef MEM_HPP
