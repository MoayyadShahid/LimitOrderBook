#include "MarketSimulator.h"

MarketSimulator::MarketSimulator(uint64_t mid_price, double price_std_dev,
                                 uint32_t min_qty, uint32_t max_qty,
                                 uint64_t seed)
    : mid_price_(mid_price),
      price_std_dev_(price_std_dev),
      min_qty_(min_qty),
      max_qty_(max_qty),
      next_id_(1),
      rng_(seed != 0 ? static_cast<std::mt19937::result_type>(seed)
                     : std::random_device{}()),
      price_dist_(static_cast<double>(mid_price), price_std_dev),
      qty_dist_(min_qty, max_qty) {}

Order MarketSimulator::generateOrder() {
    double raw_price = price_dist_(rng_);
    uint64_t price = raw_price > 0 ? static_cast<uint64_t>(raw_price + 0.5)
                                  : 1;
    if (price < 1) price = 1;

    uint32_t qty = qty_dist_(rng_);
    if (qty < min_qty_) qty = min_qty_;
    if (qty > max_qty_) qty = max_qty_;

    Side side = (rng_() & 1) ? Side::BUY : Side::SELL;
    uint64_t id = next_id_++;

    return {id, side, price, qty};
}

void MarketSimulator::generateOrders(size_t count,
                                     std::vector<Order>& out_orders) {
    out_orders.clear();
    out_orders.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        out_orders.push_back(generateOrder());
    }
}
