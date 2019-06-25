#ifndef MEM_HPP
#define MEM_HPP

/**/

#include <queue>
#include <vector>

/**/

template <typename T, std::size_t N>
struct Mem {
    /**/

    Mem() = default;

    /**/

    ~Mem() {
        for (auto it = arrays_.begin(); it != arrays_.end(); it++) {
            delete[] * it;  // NOLINT(cppcoreguidelines-owning-memory)
        }
    }

    /**/

    Mem(Mem const &) = delete;
    void operator=(Mem const &) = delete;
    Mem(Mem &&) = delete;
    void operator=(Mem &&) = delete;

    /**/

    auto Malloc() -> void * {
        if (queue_.empty()) {
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            arrays_.push_back(new char[N * sizeof(T)]);

            auto multi = (arrays_.size() - 1) * N;
            for (auto i = multi; i < multi + N; i++) {
                queue_.push(i);
            }
        }
        auto i = queue_.front();
        queue_.pop();
        auto array_no = i / N;
        auto item_no = i % N;
        return arrays_[array_no] + item_no * sizeof(T);
    }

    /**/

    void Free(void *ptr) {
        auto ptr_char = static_cast<char *>(ptr);
        for (auto it = arrays_.begin(); it != arrays_.end(); it++) {
            if (*it <= ptr_char && *it + N * sizeof(T) > ptr_char) {
                auto offset = ptr_char - *it;
                if (0 != offset % sizeof(T)) {
                    return;
                }
                queue_.push((it - arrays_.begin()) * N + offset / sizeof(T));
                return;
            }
        }
    }

    /**/

   private:
    /**/

    /**/

    std::vector<char *> arrays_;
    std::queue<int> queue_;

    /**/
};

/**/

#endif  // #ifndef MEM_HPP
