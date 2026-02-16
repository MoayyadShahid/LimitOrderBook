#include "MarketSimulator.h"
#include "OrderBook.h"
#include <chrono>
#include <iostream>
#include <string>

static std::string formatWithCommas(uint64_t n) {
    std::string s = std::to_string(n);
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i > 0 && (s.size() - i) % 3 == 0) result += ',';
        result += s[i];
    }
    return result;
}

int main() {
    constexpr size_t NUM_ORDERS = 200'000;
    constexpr uint64_t MID_PRICE = 10000;
    constexpr double PRICE_STD_DEV = 100.0;
    constexpr uint32_t MIN_QTY = 1;
    constexpr uint32_t MAX_QTY = 500;

    MarketSimulator sim(MID_PRICE, PRICE_STD_DEV, MIN_QTY, MAX_QTY, 42);
    OrderBook book;
    std::vector<Order> orders;
    std::vector<Trade> trades;
    trades.reserve(64);

    sim.generateOrders(NUM_ORDERS, orders);

    auto start = std::chrono::high_resolution_clock::now();
    for (const Order& order : orders) {
        trades.clear();
        book.addOrder(order, trades);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();
    double orders_per_sec = NUM_ORDERS / (elapsed_ms / 1000.0);

    std::cout << "Processed " << formatWithCommas(NUM_ORDERS) << " orders in "
              << elapsed_ms << " ms\n";
    std::cout << "Throughput: " << formatWithCommas(static_cast<uint64_t>(orders_per_sec))
              << " orders/sec\n";

    if (elapsed_ms < 1300.0) {
        std::cout << "Target met: < 1.3 seconds\n";
    } else {
        std::cout << "Target missed: " << (elapsed_ms / 1000.0)
                  << " s (target < 1.3 s)\n";
    }

    return 0;
}
