#ifndef MEM_HPP
#define MEM_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <list>
#include <queue>

///
template <typename T, std::size_t N>
class Mem final {
    using AlignedStorage = std::aligned_storage_t<sizeof(T), alignof(T)>;
    using ArrayStorage = std::array<char, N * sizeof(AlignedStorage)>;
    std::list<ArrayStorage> storage_{};
    std::queue<std::ptrdiff_t> indexes_{};

    auto allocStorage() {
        alignas(T) ArrayStorage arr{};
        storage_.emplace_back(std::move(arr));
        auto idx = (std::ssize(storage_) - 1) * N;
        for (auto i = 0; i < N; ++i, ++idx) { indexes_.push(idx); }
    }

public:
    ~Mem() = default;
    Mem() = default;
    Mem(Mem const &) = delete;
    Mem(Mem &&) = delete;

    void operator=(Mem const &) = delete;
    void operator=(Mem &&) = delete;

    ///
    auto Malloc() -> void * {
        if (indexes_.empty()) { allocStorage(); }
        assert(!indexes_.empty());
        auto idx = indexes_.front();
        indexes_.pop();
        auto slot = std::begin(storage_);
        std::advance(slot, idx / N);
        auto block = std::begin(*slot) + (idx % N) * sizeof(AlignedStorage);
        return &(*block);
    }

    ///
    void Free(void *ptr) {
        auto slot = std::find_if(std::begin(storage_), std::end(storage_), [&ptr](auto const &slot) {
            auto distance = static_cast<char *>(ptr) - slot.data();
            return distance >= 0 && distance < N * sizeof(AlignedStorage);
        });
        assert(slot != std::end(storage_));
        auto distance = static_cast<char *>(ptr) - slot->data();
        assert(distance >= 0 && distance < N * sizeof(AlignedStorage) && distance % sizeof(AlignedStorage) == 0);
        indexes_.push(std::distance(std::begin(storage_), slot) * N + distance / sizeof(AlignedStorage));
    }
};

/**/

#endif  // #ifndef MEM_HPP
