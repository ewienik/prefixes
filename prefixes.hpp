#ifndef PREFIXES_HPP
#define PREFIXES_HPP

/**/

char const kMaskNotFound = -1;

/**/

struct Prefixes {

    /**/

    Prefixes();
    ~Prefixes();

    /**/

    void Add(int ip, char mask);
    void Del(int ip, char mask);
    char Check(int ip);

    /**/

private:

    /**/

    struct Opaque;
    Opaque * opaque_;

    /**/

};

/**/

#endif // #ifndef PREFIXES_HPP
