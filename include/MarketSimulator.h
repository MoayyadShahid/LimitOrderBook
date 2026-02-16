#pragma once

#include "Types.h"
#include <random>
#include <vector>

class MarketSimulator {
public:
    MarketSimulator(uint64_t mid_price, double price_std_dev, uint32_t min_qty,
                    uint32_t max_qty, uint64_t seed = 0);

    Order generateOrder();

    void generateOrders(size_t count, std::vector<Order>& out_orders);

private:
    uint64_t mid_price_;
    double price_std_dev_;
    uint32_t min_qty_;
    uint32_t max_qty_;
    uint64_t next_id_;
    std::mt19937 rng_;
    std::normal_distribution<double> price_dist_;
    std::uniform_int_distribution<uint32_t> qty_dist_;
};
