#include "OrderBook.h"

void OrderBook::addOrder(const Order& order, std::vector<Trade>& out_trades) {
    if (order.quantity == 0) {
        return;
    }

    uint32_t remaining_qty = order.quantity;

    if (order.side == Side::BUY) {
        // Match against asks (lowest price first)
        while (remaining_qty > 0 && !asks_.empty()) {
            auto ask_it = asks_.begin();
            const uint64_t best_ask_price = ask_it->first;
            if (best_ask_price > order.price) {
                break;
            }

            PriceLevel& level = ask_it->second;
            Order& resting = level.orders.front();
            const uint32_t fill_qty =
                remaining_qty < resting.quantity ? remaining_qty : resting.quantity;

            out_trades.push_back(
                {order.id, resting.id, best_ask_price, fill_qty});

            resting.quantity -= fill_qty;
            level.total_volume -= fill_qty;
            remaining_qty -= fill_qty;

            if (resting.quantity == 0) {
                level.orders.pop_front();
                if (level.orders.empty()) {
                    asks_.erase(ask_it);
                }
            } else {
                break;
            }
        }

        // Add remainder to bids
        if (remaining_qty > 0) {
            Order remainder = order;
            remainder.quantity = remaining_qty;
            auto& level = bids_[order.price];
            level.orders.push_back(remainder);
            level.total_volume += remaining_qty;
        }
    } else {
        // SELL: match against bids (highest price first)
        while (remaining_qty > 0 && !bids_.empty()) {
            auto bid_it = bids_.begin();
            const uint64_t best_bid_price = bid_it->first;
            if (best_bid_price < order.price) {
                break;
            }

            PriceLevel& level = bid_it->second;
            Order& resting = level.orders.front();
            const uint32_t fill_qty =
                remaining_qty < resting.quantity ? remaining_qty : resting.quantity;

            out_trades.push_back(
                {resting.id, order.id, best_bid_price, fill_qty});

            resting.quantity -= fill_qty;
            level.total_volume -= fill_qty;
            remaining_qty -= fill_qty;

            if (resting.quantity == 0) {
                level.orders.pop_front();
                if (level.orders.empty()) {
                    bids_.erase(bid_it);
                }
            } else {
                break;
            }
        }

        // Add remainder to asks
        if (remaining_qty > 0) {
            Order remainder = order;
            remainder.quantity = remaining_qty;
            auto& level = asks_[order.price];
            level.orders.push_back(remainder);
            level.total_volume += remaining_qty;
        }
    }
}

std::vector<PriceVolume> OrderBook::getTopBids(size_t n) const {
    std::vector<PriceVolume> result;
    result.reserve(n);
    for (auto it = bids_.begin(); it != bids_.end() && result.size() < n; ++it) {
        result.push_back({it->first, it->second.total_volume});
    }
    return result;
}

std::vector<PriceVolume> OrderBook::getTopAsks(size_t n) const {
    std::vector<PriceVolume> result;
    result.reserve(n);
    for (auto it = asks_.begin(); it != asks_.end() && result.size() < n; ++it) {
        result.push_back({it->first, it->second.total_volume});
    }
    return result;
}
