
/**/

#include "prefixes.hpp"

#include "mem.hpp"

/**/

#include <new>

/**/

namespace {

constexpr auto kMemCount = 100000;
constexpr auto kBits32 = 32;

///
struct BitIp {
    int bit_{};                       // NOLINT(misc-non-private-member-variables-in-classes)
    bool added_{};                    // NOLINT(misc-non-private-member-variables-in-classes)
    BitIp* up_{};                     // NOLINT(misc-non-private-member-variables-in-classes)
    std::unique_ptr<BitIp> true_{};   // NOLINT(misc-non-private-member-variables-in-classes)
    std::unique_ptr<BitIp> false_{};  // NOLINT(misc-non-private-member-variables-in-classes)

    ~BitIp() = default;
    BitIp(int bit = kBits32, BitIp* parent = nullptr) : bit_(bit), up_(parent) {}
    BitIp(BitIp const&) = delete;
    BitIp(BitIp&&) = default;
    auto operator=(BitIp const&) = delete;
    auto operator=(BitIp &&) -> BitIp& = default;

    static auto operator new(size_t /*count*/) -> void* { return mem().Malloc(); }
    static void operator delete(void* ptr) { mem().Free(ptr); }

private:
    static auto mem() -> Mem<BitIp, kMemCount>& {
        static Mem<BitIp, kMemCount> mem_;
        return mem_;
    }
};

///
constexpr auto BitMin(int mask) {
    if (0 > mask) {
        mask = 0;
    } else if (kBits32 < mask) {
        mask = kBits32;
    }
    return kBits32 - mask;
}

///
auto Closest(BitIp* bit_ip, int ip, int bit_min) {
    for (int bit = kBits32 - 1; bit >= bit_min; bit--) {
        if (0 == (ip & (1 << bit))) {
            if (!bit_ip->false_) { return bit_ip; }
            bit_ip = bit_ip->false_.get();
            continue;
        }
        if (!bit_ip->true_) { return bit_ip; }
        bit_ip = bit_ip->true_.get();
    }
    return bit_ip;
}

///
void Add(BitIp* closest, int ip, int bit_min) {
    for (int bit = closest->bit_ - 1; bit >= bit_min; bit--) {
        if (0 == (ip & (1 << bit))) {
            closest->false_ = std::make_unique<BitIp>(bit, closest);
            closest = closest->false_.get();
            continue;
        }
        closest->true_ = std::make_unique<BitIp>(bit, closest);
        closest = closest->true_.get();
    }
    closest->added_ = true;
}

///
void Delete(BitIp* closest, int bit_min) {
    if (closest->bit_ > bit_min) { return; }
    closest->added_ = false;
    while (nullptr != closest->up_ && !closest->added_ && nullptr == closest->true_ && !closest->false_) {
        BitIp* up = closest->up_;
        if (up->true_.get() == closest) {
            up->true_.reset();
        } else {
            up->false_.reset();
        }
        closest = up;
    }
}

///
auto Find(BitIp* closest) -> BitIp* {
    while (nullptr != closest && !closest->added_) { closest = closest->up_; }
    return closest;
}

///
auto Mask(BitIp* bit_ip) -> int {
    if (nullptr == bit_ip) { return kMaskNotFound; }
    return kBits32 - bit_ip->bit_;
}

}  // namespace

/**/

struct Prefixes::Opaque {
    std::unique_ptr<BitIp> base{std::make_unique<BitIp>()};
};

/**/

Prefixes::Prefixes() : opaque_(new Opaque) {}

/**/

Prefixes::~Prefixes() = default;

/**/

void Prefixes::Add(int ip, int mask) {
    auto bit_min = BitMin(mask);
    ::Add(::Closest(opaque_->base.get(), ip, bit_min), ip, bit_min);
}

/**/

void Prefixes::Del(int ip, int mask) {
    auto bit_min = BitMin(mask);
    ::Delete(::Closest(opaque_->base.get(), ip, bit_min), bit_min);
}

/**/

auto Prefixes::Check(int ip) -> int { return ::Mask(::Find(::Closest(opaque_->base.get(), ip, BitMin(kBits32)))); }

/**/

