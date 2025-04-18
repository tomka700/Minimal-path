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

#define n 8
#define MAX_LEN 25

constexpr int total_bits = n * n;

struct Dir {
    int dx;
    int dy;
    int max_added;
};

constexpr Dir dirs[8] = {
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
using MaskType = std::array<std::array<std::bitset<total_bits>, n+1>, n+1>;

constexpr MaskType create_vertex_masks() {
    MaskType masks;
    for (int x = 0; x <= n; ++x) {
        for (int y = 0; y <= n; ++y) {
            std::bitset<total_bits> mask;
            if (x > 0 && y > 0) mask.set((x-1) * n + (y-1));
            if (x > 0 && y < n) mask.set((x-1) * n + y);
            if (x < n && y > 0) mask.set(x * n + (y-1));
            if (x < n && y < n) mask.set(x * n + y);
            masks[x][y] = mask;
        }
    }
    return masks;
}
constexpr auto vertex_masks = create_vertex_masks();

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
constexpr auto is_outer = create_is_outer();

std::atomic<int> global_best = MAX_LEN;
std::atomic<bool> found = false;
std::mutex global_mutex;
/*
bool are_zeros_connected(std::bitset<total_bits>& mask) {
    return false
}
*/
inline void dfs(std::bitset<total_bits>& mask, std::vector<std::pair<int, int>>& path) {
    const int len = path.size() - 1;
    const int count = mask.count();
    const int current_best = global_best.load(std::memory_order_relaxed);
    if (len + (total_bits - count + 2) / 3 > current_best) [[likely]] return;

    if (count == total_bits) [[unlikely]] {
        // permissive in order to print all optimal solutions
        if (len <= current_best) [[unlikely]] {
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
    for (const auto& dir : dirs) {
        const int nx = x + dir.dx;
        const int ny = y + dir.dy;
        if (is_outer[nx][ny]) [[unlikely]] continue;

        auto added = vertex_masks[nx][ny] & invmask;
        if (n != 3 ? static_cast<int>(added.count()) < dir.max_added : ((dir.dx != 0 && dir.dy != 0) || static_cast<int>(added.count()) == 0)) [[likely]] continue;
        
        path.push_back({nx, ny});
        mask |= vertex_masks[nx][ny];
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
    std::bitset<total_bits> mask;
    for (const auto& p : path) {
        mask |= vertex_masks[p.first][p.second];
    }
    std::vector<std::pair<int, int>> local_path = path;
    local_path.reserve(MAX_LEN + 1);
    dfs(mask, local_path);
}

inline void run_parallel_search(const std::vector<std::vector<std::pair<int, int>>>& paths) {
    const int hw_concurrency = static_cast<int>(std::thread::hardware_concurrency());
    const int num_threads = paths.empty() ? 0 : std::clamp(hw_concurrency, 1, static_cast<int>(paths.size()));
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
    constexpr int max_x = (n < 3) ? 1 : n / 2;
    for (int x = 1; x <= max_x; ++x) {
        for (int y = 1; y <= x; ++y) {
            if ((n > 3 && x == max_x && y == max_x) || (x == 2 && y == 2)) continue;

            paths.emplace_back();
            paths.back().reserve(MAX_LEN + 1);
            paths.back().push_back({x, y});
        }
    }
    
    force_obvious_moves(paths);
    run_parallel_search(paths);
    
    if (!found) {
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
