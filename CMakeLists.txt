cmake_minimum_required(VERSION 3.22)

# Fix macOS toolchain: ensure SDK and C++ headers are found
if(APPLE)
    execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_OSX_SYSROOT "${SDK_PATH}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot ${SDK_PATH} -I${SDK_PATH}/usr/include/c++/v1 -stdlib=libc++")
endif()

project(LimitOrderBook
    LANGUAGES CXX
    VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Fetch Dependencies ------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

# FTXUI (for Phase 4 terminal UI)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ftxui)

# --- Library ------------------------------------------------------------------
add_library(LimitOrderBook STATIC
    src/OrderBook.cpp
    src/MarketSimulator.cpp
)
target_include_directories(LimitOrderBook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Main Executable (FTXUI Dashboard) ----------------------------------------
add_executable(lob_main src/main.cpp)
target_link_libraries(lob_main PRIVATE
    LimitOrderBook
    ftxui::component
    ftxui::dom
    ftxui::screen
)

# --- Benchmark (Phase 3) - simple chrono-based, no external deps --------------
option(BUILD_BENCHMARKS "Build benchmark target (lob_benchmark)" ON)
if(BUILD_BENCHMARKS)
    add_executable(lob_benchmark tests/benchmark.cpp)
    target_link_libraries(lob_benchmark PRIVATE LimitOrderBook)
endif()
