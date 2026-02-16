#include "MarketSimulator.h"
#include "OrderBook.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <chrono>
#include <deque>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

using namespace ftxui;

constexpr size_t MAX_TRADES_DISPLAY = 30;
constexpr size_t TOP_LEVELS = 30;
constexpr size_t BATCH_SIZE = 500;
constexpr uint64_t MID_PRICE = 10000;
constexpr double PRICE_STD_DEV = 150.0;
constexpr uint32_t MIN_QTY = 1;
constexpr uint32_t MAX_QTY = 200;

struct AppState {
    std::mutex mtx;
    OrderBook book;
    std::deque<Trade> recent_trades;
    MarketSimulator sim{MID_PRICE, PRICE_STD_DEV, MIN_QTY, MAX_QTY, 0};
    uint64_t order_count{0};
    bool running{true};
};

std::string formatPrice(uint64_t price) {
    std::ostringstream oss;
    oss << (price / 100) << "." << (price % 100 / 10) << (price % 10);
    return oss.str();
}

Component buildDashboard(AppState& state) {
    return Renderer([&state] {
        std::lock_guard<std::mutex> lock(state.mtx);

        auto bids = state.book.getTopBids(TOP_LEVELS);
        auto asks = state.book.getTopAsks(TOP_LEVELS);

        Elements bid_rows;
        bid_rows.push_back(text(" PRICE    VOLUME ") | bold);
        bid_rows.push_back(separator());
        for (const auto& pv : bids) {
            bid_rows.push_back(
                text(" " + formatPrice(pv.price) + "   " +
                     std::to_string(pv.volume) + " ") |
                color(Color::Green));
        }
        for (size_t i = bids.size(); i < TOP_LEVELS; ++i) {
            bid_rows.push_back(text(" -        - "));
        }

        Elements ask_rows;
        ask_rows.push_back(text(" PRICE    VOLUME ") | bold);
        ask_rows.push_back(separator());
        for (const auto& pv : asks) {
            ask_rows.push_back(
                text(" " + formatPrice(pv.price) + "   " +
                     std::to_string(pv.volume) + " ") |
                color(Color::Red));
        }
        for (size_t i = asks.size(); i < TOP_LEVELS; ++i) {
            ask_rows.push_back(text(" -        - "));
        }

        Elements trade_rows;
        trade_rows.push_back(text(" BUY  SELL  PRICE   QTY ") | bold);
        trade_rows.push_back(separator());
        for (auto it = state.recent_trades.rbegin();
             it != state.recent_trades.rend(); ++it) {
            trade_rows.push_back(
                text(" " + std::to_string(it->buy_id) + "    " +
                     std::to_string(it->sell_id) + "   " +
                     formatPrice(it->price) + "  " +
                     std::to_string(it->quantity) + " "));
        }

        auto bids_panel =
            vbox(bid_rows) | border | flex | bgcolor(Color::Black);
        auto asks_panel =
            vbox(ask_rows) | border | flex | bgcolor(Color::Black);
        auto trades_panel =
            vbox(trade_rows) | border | flex | bgcolor(Color::Black);

        return hbox({
            vbox({
                text(" BIDS ") | bold | color(Color::Green),
                bids_panel,
            }) | flex,
            vbox({
                text(" ASKS ") | bold | color(Color::Red),
                asks_panel,
            }) | flex,
            vbox({
                text(" EXECUTIONS ") | bold,
                trades_panel,
            }) | flex,
        });
    });
}

void simulatorThread(AppState& state, ScreenInteractive& screen) {
    std::vector<Order> batch;
    batch.reserve(BATCH_SIZE);
    std::vector<Trade> trades;
    trades.reserve(64);

    while (state.running) {
        batch.clear();
        for (size_t i = 0; i < BATCH_SIZE && state.running; ++i) {
            batch.push_back(state.sim.generateOrder());
        }

        {
            std::lock_guard<std::mutex> lock(state.mtx);
            for (const Order& order : batch) {
                trades.clear();
                state.book.addOrder(order, trades);
                state.order_count += 1;
                for (const Trade& t : trades) {
                    state.recent_trades.push_back(t);
                    if (state.recent_trades.size() > MAX_TRADES_DISPLAY) {
                        state.recent_trades.pop_front();
                    }
                }
            }
        }

        screen.Post(Event::Custom);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    AppState state;

    auto dashboard = buildDashboard(state);
    auto component = CatchEvent(dashboard, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            state.running = false;
            screen.Exit();
            return true;
        }
        return false;
    });

    std::thread sim_thread(simulatorThread, std::ref(state), std::ref(screen));

    screen.Loop(component);

    state.running = false;
    sim_thread.join();

    return 0;
}
