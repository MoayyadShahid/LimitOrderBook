#pragma once

#include "Types.h"
#include <map>
#include <vector>

class OrderBook {
public:
    void addOrder(const Order& order, std::vector<Trade>& out_trades);

    std::vector<PriceVolume> getTopBids(size_t n) const;
    std::vector<PriceVolume> getTopAsks(size_t n) const;

private:
    using BidsMap = std::map<uint64_t, PriceLevel, std::greater<uint64_t>>;
    using AsksMap = std::map<uint64_t, PriceLevel, std::less<uint64_t>>;
    BidsMap bids_;
    AsksMap asks_;
};
