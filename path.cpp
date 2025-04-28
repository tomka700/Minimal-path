#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>

#define n 8
#define ONLY_PROVE_LENGTH false

static_assert(n > 0, "n must be positive!");

constexpr int TOTAL_BITS = n * n;

constexpr int calculate_current_best() {
    switch (n) {
    case 1:
    case 2:
        return 0;
    case 3:
        return 3;
    case 4:
        return 6;
    }

    switch (n % 3) {
    case 0:
        return (2 * n - 2) + (n * (n - 4)) / 3;
    case 1:
        return (2 * n - 2 + 3) + (n * (n - 4) - 6) / 3;
    case 2:
        return (2 * n - 2 + 1) + (n * (n - 4) - 2) / 3;
    }
}
constexpr int CURRENT_BEST = calculate_current_best();
constexpr int MAX_LEN = std::max(0, ONLY_PROVE_LENGTH ? CURRENT_BEST - 1 : CURRENT_BEST);

struct Dir {
    int dx;
    int dy;
    int max_added;
};

constexpr Dir DIRS[8] = {
    {-1, 1, 3},
    {-1, -1, 3},
    {1, -1, 3},
    {1, 1, 3},
    {0, -1, 2},
    {1, 0, 2},
    {0, 1, 2},
    {-1, 0, 2}
};
// matrix of bitmasks that are only true for every vertex's surrounding squares
using MaskType = std::array<std::array<std::bitset<TOTAL_BITS>, n+1>, n+1>;

constexpr MaskType create_vertex_masks() {
    MaskType masks;
    for (int x = 0; x <= n; ++x) {
        for (int y = 0; y <= n; ++y) {
            std::bitset<TOTAL_BITS> mask;
            if (x > 0 && y > 0) mask.set((x-1) * n + (y-1));
            if (x > 0 && y < n) mask.set((x-1) * n + y);
            if (x < n && y > 0) mask.set(x * n + (y-1));
            if (x < n && y < n) mask.set(x * n + y);
            masks[x][y] = mask;
        }
    }
    return masks;
}
constexpr auto VERTEX_MASKS = create_vertex_masks();

constexpr std::array<std::array<bool, n+1>, n+1> create_is_outer() {
    std::array<std::array<bool, n+1>, n+1> is_outer;
    for (int i = 0; i <= n; ++i) {
        is_outer[i][0] = true;
        is_outer[i][n] = true;
        is_outer[0][i] = true;
        is_outer[n][i] = true;
    }
    return is_outer;
}
constexpr auto IS_OUTER = create_is_outer();

std::atomic<int> global_best = MAX_LEN;
std::atomic<bool> found = false;
std::mutex global_mutex;

inline void dfs(std::bitset<TOTAL_BITS>& mask, std::vector<std::pair<int, int>>& path) {
    const int len = path.size() - 1;
    const int count = mask.count();
    const int best_len = global_best.load(std::memory_order_relaxed);
    if (len + (TOTAL_BITS - count + 2) / 3 > best_len) [[likely]] return;

    if (count == TOTAL_BITS) [[unlikely]] {
        // permissive in order to print all optimal solutions
        if (len <= best_len) [[unlikely]] {
            std::lock_guard<std::mutex> lock(global_mutex);
            if (len <= global_best) {
                found = true;
                global_best = len;
                std::cout << "New best: " << len << ", Path: ";
                for (const auto& p : path) {
                    std::cout << "(" << p.first << "," << p.second << ")";
                }
                std::cout << "\n";
            }
        }
        return;
    }
    const auto& [x, y] = path.back();
    const auto& invmask = ~mask;
    for (const auto& dir : DIRS) {
        const int nx = x + dir.dx;
        const int ny = y + dir.dy;
        if (IS_OUTER[nx][ny]) [[unlikely]] continue;

        auto added = VERTEX_MASKS[nx][ny] & invmask;
        if (n != 3 ? static_cast<int>(added.count()) < dir.max_added : ((dir.dx != 0 && dir.dy != 0) || static_cast<int>(added.count()) == 0)) [[likely]] continue;
        
        path.push_back({nx, ny});
        mask |= VERTEX_MASKS[nx][ny];
        dfs(mask, path);
        mask &= ~added;
        path.pop_back();
    }
}

inline void force_obvious_moves(std::vector<std::vector<std::pair<int, int>>>& paths) {
    if (n < 3) return;

    for (auto& path : paths) {
        auto& [x, y] = path.back();

        auto step = [&](int nx, int ny) {
            path.push_back({nx, ny});
        };

        switch (y) {
        case 2:
            step(x, 1);
        case 1:
            switch (x) {
            case 4:
                step(3, 2);
            case 3:
                step(2, 1);
            case 2:
                step(1, 1);
            case 1:
                step(1, 2);
            }
        }
    }
}

inline void search_from(const std::vector<std::pair<int, int>>& path) {
    std::bitset<TOTAL_BITS> mask;
    for (const auto& p : path) {
        mask |= VERTEX_MASKS[p.first][p.second];
    }
    std::vector<std::pair<int, int>> local_path = path;
    local_path.reserve(MAX_LEN + 1);
    dfs(mask, local_path);
}

inline void run_parallel_search(const std::vector<std::vector<std::pair<int, int>>>& paths) {
    assert(!paths.empty() && "paths must not be empty!");
    const int hw_concurrency = static_cast<int>(std::thread::hardware_concurrency());
    assert(hw_concurrency > 0 && "No supported threads!");
    const int num_threads = std::min(hw_concurrency, static_cast<int>(paths.size()));
    std::vector<std::jthread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([i, num_threads, &paths] {
            for (int j = i; j < static_cast<int>(paths.size()); j += num_threads) {
                search_from(paths[j]);
            }
        });
    }
}

int main() {
    std::vector<std::vector<std::pair<int, int>>> paths;
    constexpr int MAX_X = std::max(1, n / 2);
    for (int x = 1; x <= MAX_X; ++x) {
        for (int y = 1; y <= x; ++y) {
            if ((n > 3 && x == MAX_X && y == MAX_X) || (x == 2 && y == 2)) continue;
            paths.emplace_back();
            paths.back().reserve(MAX_LEN + 1);
            paths.back().push_back({x, y});
        }
    }
    
    force_obvious_moves(paths);
    run_parallel_search(paths);
    
    if (!found) {
        if (ONLY_PROVE_LENGTH) {
            std::cout << "The optimal path length for n = " << n << " is at least " << CURRENT_BEST << ".\n";
        }
        std::cout << "No solution found.";
    }
    /*
    long long total_runtime = 0;
    int runs = 5;
    for (int i = 0; i < runs; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();
        {
            run_parallel_search(starts);
            global_best = MAX_LEN;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        total_runtime += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }

    double average_runtime = static_cast<double>(total_runtime) / runs;
    std::cout << "Average Runtime: " << average_runtime << " ms" << std::endl;
    */
    return 0;
}
