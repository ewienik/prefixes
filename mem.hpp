#ifndef MEM_HPP
#define MEM_HPP

/**/

#include <queue>
#include <vector>

/**/

template <typename T, std::size_t N>
struct Mem {
    /**/

    Mem() {}

    /**/

    ~Mem() {
        for (std::vector<char *>::const_iterator it = arrays_.begin();
             it != arrays_.end(); it++) {
            delete[] * it;
        }
    }

    /**/

    void *Malloc() {
        if (queue_.empty()) {
            arrays_.push_back(new char[N * sizeof(T)]);
            int multi = (arrays_.size() - 1) * N;
            for (int i = multi; i < multi + N; i++) {
                queue_.push(i);
            }
        }
        int i = queue_.front();
        queue_.pop();
        int array_no = i / N;
        int item_no = i % N;
        return arrays_[array_no] + item_no * sizeof(T);
    }

    /**/

    void Free(void *ptr) {
        char *ptr_char = static_cast<char *>(ptr);
        for (std::vector<char *>::const_iterator it = arrays_.begin();
             it != arrays_.end(); it++) {
            if (*it <= ptr_char && *it + N * sizeof(T) > ptr_char) {
                int offset = ptr_char - *it;
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

    Mem(Mem const &);
    void operator=(Mem const &);

    /**/

    std::vector<char *> arrays_;
    std::queue<int> queue_;

    /**/
};

/**/

#endif  // #ifndef MEM_HPP
