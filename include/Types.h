#pragma once

#include <stdint.h>
#include <deque>

enum class Side : uint8_t {
    BUY,
    SELL
};

struct Order {
    uint64_t id;
    Side side;
    uint64_t price;   // e.g. 10050 = $100.50
    uint32_t quantity;
};

struct Trade {
    uint64_t buy_id;
    uint64_t sell_id;
    uint64_t price;
    uint32_t quantity;
};

struct PriceLevel {
    std::deque<Order> orders;
    uint32_t total_volume;
};

struct PriceVolume {
    uint64_t price;
    uint32_t volume;
};
