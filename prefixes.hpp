#ifndef PREFIXES_HPP
#define PREFIXES_HPP

/**/

#include <memory>

/**/

int const kMaskNotFound = -1;

/**/

struct Prefixes {
    /**/

    Prefixes();
    ~Prefixes();

    Prefixes(Prefixes const &) = delete;
    void operator=(Prefixes const &) = delete;
    Prefixes(Prefixes &&) = delete;
    void operator=(Prefixes &&) = delete;

    /**/

    void Add(int ip, int mask);
    void Del(int ip, int mask);
    auto Check(int ip) -> int;

    /**/

   private:
    /**/

    struct Opaque;
    std::unique_ptr<Opaque> opaque_;

    /**/
};

/**/

#endif  // #ifndef PREFIXES_HPP
