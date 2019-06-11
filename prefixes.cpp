
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
constexpr int kBits32 = 32;

/**/

struct BitIp;
Mem<BitIp, kMemCount> mem;

/**/

struct BitIp {  // NOLINT(cppcoreguidelines-special-member-functions)
    /**/

    int bit_{};       // NOLINT(misc-non-private-member-variables-in-classes)
    bool added_{};    // NOLINT(misc-non-private-member-variables-in-classes)
    BitIp* up_{};     // NOLINT(misc-non-private-member-variables-in-classes)
    BitIp* true_{};   // NOLINT(misc-non-private-member-variables-in-classes)
    BitIp* false_{};  // NOLINT(misc-non-private-member-variables-in-classes)

    /**/

    BitIp(int bit, bool added, BitIp* up) : bit_(bit), added_(added), up_(up) {}

    /**/

    ~BitIp() {
        delete true_;
        delete false_;
    }

    /**/

    static auto operator new(size_t /*count*/) -> void* { return mem.Malloc(); }

    /**/

    static void operator delete(void* ptr) { mem.Free(ptr); }

    /**/
};

/**/

}  // namespace

/**/

struct Prefixes::Opaque {  // NOLINT(cppcoreguidelines-special-member-functions)

    /**/

    BitIp* base_{};  // NOLINT(misc-non-private-member-variables-in-classes)

    /**/

    Opaque() : base_(new BitIp(kBits32, false, nullptr)) {}

    /**/

    ~Opaque() { delete base_; }

    /**/

    auto BitMin(int mask) -> int {
        if (0 > mask) {
            mask = 0;
        } else if (kBits32 < mask) {
            mask = kBits32;
        }
        return kBits32 - mask;
    }

    /**/

    auto Closest(int ip, int bit_min) -> BitIp* {
        auto bit_ip = base_;
        for (int bit = kBits32 - 1; bit >= bit_min; bit--) {
            if (0 == (ip & (1 << bit))) {
                if (nullptr == bit_ip->false_) {
                    return bit_ip;
                }
                bit_ip = bit_ip->false_;
                continue;
            }
            if (nullptr == bit_ip->true_) {
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
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                closest->false_ = new BitIp(bit, false, closest);
                closest = closest->false_;
                continue;
            }
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
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
        while (nullptr != closest->up_ && !closest->added_ &&
               nullptr == closest->true_ && nullptr == closest->false_) {
            BitIp* up = closest->up_;
            if (up->true_ == closest) {
                up->true_ = nullptr;
            } else {
                up->false_ = nullptr;
            }
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            delete closest;
            closest = up;
        }
    }

    /**/

    auto Find(BitIp* closest) -> BitIp* {
        while (nullptr != closest && !closest->added_) {
            closest = closest->up_;
        }
        return closest;
    }

    /**/

    auto Mask(BitIp* bit_ip) -> int {
        if (nullptr == bit_ip) {
            return kMaskNotFound;
        }
        return kBits32 - bit_ip->bit_;
    }

    /**/
};

/**/

Prefixes::Prefixes() : opaque_(new Opaque) {}

/**/

Prefixes::~Prefixes() { delete opaque_; }

/**/

void Prefixes::Add(int ip, int mask) {
    auto bit_min = opaque_->BitMin(mask);
    opaque_->Add(opaque_->Closest(ip, bit_min), ip, bit_min);
}

/**/

void Prefixes::Del(int ip, int mask) {
    auto bit_min = opaque_->BitMin(mask);
    opaque_->Delete(opaque_->Closest(ip, bit_min), bit_min);
}

/**/

auto Prefixes::Check(int ip) -> int {
    return opaque_->Mask(
        opaque_->Find(opaque_->Closest(ip, opaque_->BitMin(kBits32))));
}

/**/

