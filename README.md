# Limit Order Book

A high-performance limit order book matching engine in C++20. Price-time priority, market simulator, and a real-time terminal UI.

## Demo

https://github.com/user-attachments/assets/5a0aafb6-c9ea-4696-8b91-21723dfe8ed6


## Building

Requires CMake 3.22+ and a C++20 compiler. Dependencies (FTXUI) are fetched automatically.

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

On macOS, Xcode Command Line Tools or full Xcode is required. If you run into header issues, `brew install llvm` and use `-DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++`.

## Running

**Live dashboard** — bids, asks, and executions updating in real time. Press `q` or Escape to quit.

```bash
./lob_main
```

**Benchmark** — processes 200,000 orders and reports throughput. Build with `-DBUILD_BENCHMARKS=ON` (default).

```bash
./lob_benchmark
```

Typical output: ~1.5M orders/sec on an M-series Mac.

## Project structure

```
include/     — Types, OrderBook, MarketSimulator
src/         — Implementation + main UI
tests/       — Benchmark
```

## Approach

The matching engine avoids heap allocations in the hot path, uses `std::deque` for order queues at each price level, and passes trades out via a pre-allocated vector reference. Prices are stored as fixed-point integers (e.g. 10050 = $100.50) to avoid floating-point issues.
