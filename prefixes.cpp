
/**/

#include "prefixes.hpp"

#include "mem.hpp"

/**/

#include <new>

/**/

using namespace std;

/**/

namespace {

/**/

size_t const kMemCount = 100000;

/**/

struct BitIp;
Mem<BitIp, kMemCount> mem;

/**/

struct BitIp {
    /**/

    int bit_;
    bool added_;
    BitIp* up_;
    BitIp* true_;
    BitIp* false_;

    /**/

    BitIp(int bit, bool added, BitIp* up)
        : bit_(bit), added_(added), up_(up), true_(0), false_(0) {}

    /**/

    ~BitIp() {
        if (0 != true_) {
            delete true_;
        }
        if (0 != false_) {
            delete false_;
        }
    }

    /**/

    static void* operator new(size_t count) { return mem.Malloc(); }

    /**/

    static void operator delete(void* ptr) { mem.Free(ptr); }

    /**/
};

/**/

}  // namespace

/**/

struct Prefixes::Opaque {
    /**/

    BitIp* base_;

    /**/

    Opaque() : base_(new BitIp(32, false, 0)) {}

    /**/

    ~Opaque() {
        if (0 != base_) {
            delete base_;
        }
    }

    /**/

    int BitMin(char mask) {
        if (0 > mask) {
            mask = 0;
        } else if (32 < mask) {
            mask = 32;
        }
        return 32 - mask;
    }

    /**/

    BitIp* Closest(int ip, int bit_min) {
        BitIp* bit_ip = base_;
        for (int bit = 31; bit >= bit_min; bit--) {
            if (0 == (ip & (1 << bit))) {
                if (0 == bit_ip->false_) {
                    return bit_ip;
                }
                bit_ip = bit_ip->false_;
                continue;
            }
            if (0 == bit_ip->true_) {
                return bit_ip;
            }
            bit_ip = bit_ip->true_;
        }
        return bit_ip;
    }

    /**/

    void Add(BitIp* closest, int ip, int bit_min) {
        for (int bit = closest->bit_ - 1; bit >= bit_min; bit--) {
            if (0 == (ip & (1 << bit))) {
                closest->false_ = new BitIp(bit, false, closest);
                closest = closest->false_;
                continue;
            }
            closest->true_ = new BitIp(bit, false, closest);
            closest = closest->true_;
        }
        closest->added_ = true;
    }

    /**/

    void Delete(BitIp* closest, int bit_min) {
        if (closest->bit_ > bit_min) {
            return;
        }
        closest->added_ = false;
        while (0 != closest->up_ && !closest->added_ && 0 == closest->true_ &&
               0 == closest->false_) {
            BitIp* up = closest->up_;
            if (up->true_ == closest) {
                up->true_ = 0;
            } else {
                up->false_ = 0;
            }
            delete closest;
            closest = up;
        }
    }

    /**/

    BitIp* Find(BitIp* closest) {
        while (0 != closest && !closest->added_) {
            closest = closest->up_;
        }
        return closest;
    }

    /**/

    char Mask(BitIp* bit_ip) {
        if (0 == bit_ip) {
            return kMaskNotFound;
        }
        return 32 - bit_ip->bit_;
    }

    /**/
};

/**/

Prefixes::Prefixes() : opaque_(new Opaque) {}

/**/

Prefixes::~Prefixes() { delete opaque_; }

/**/

void Prefixes::Add(int ip, char mask) {
    int bit_min = opaque_->BitMin(mask);
    opaque_->Add(opaque_->Closest(ip, bit_min), ip, bit_min);
}

/**/

void Prefixes::Del(int ip, char mask) {
    int bit_min = opaque_->BitMin(mask);
    opaque_->Delete(opaque_->Closest(ip, bit_min), bit_min);
}

/**/

char Prefixes::Check(int ip) {
    return opaque_->Mask(
        opaque_->Find(opaque_->Closest(ip, opaque_->BitMin(32))));
}

/**/

