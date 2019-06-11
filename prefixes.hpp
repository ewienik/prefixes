#ifndef PREFIXES_HPP
#define PREFIXES_HPP

/**/

int const kMaskNotFound = -1;

/**/

struct Prefixes {
    /**/

    Prefixes();
    ~Prefixes();

    /**/

    void Add(int ip, int mask);
    void Del(int ip, int mask);
    int Check(int ip);

    /**/

   private:
    /**/

    struct Opaque;
    Opaque* opaque_;

    /**/
};

/**/

#endif  // #ifndef PREFIXES_HPP
