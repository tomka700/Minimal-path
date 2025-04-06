// g++ -std=c++2b -Ofast -march=native -flto path.cpp -o path
/*
 3  0  1
 6  0  1
 9  1  1
10  4  3
15  5  6
15 10  3
16 15  9
21 18?12?
21 25? 8?
22 32? 6?
27 37?22?
27 46? 6?
28 55?
33 62?
36 71?
34 84? 5?
# straights, # diagonals, # unique optimal solutions
n = nth line of the file, ? = unproven
*/

/*
guide to running optimally:
- close everything else that would waste CPU resources
- set n
- set MAX_LEN to the current best (sum of the first two numbers in the above matrix)
- use the first line of the file to compile
*/

/*
notes:
- this code only tests (1, 1) and (1, 0) moves
- Chebysev distance is used as 3/1 > 2/1, just like 3/sqrt2 > 2/1 and it is faster
- all moves of optimal solutions for 3 < n < 8 are zero waste, this is enforced as a general rule, see added_count
- almost all current best solutions start or end at (1, 1) or (1, 2),
  n = 12, 15 break this pattern, but those solutions are transformable into ones that do follow it
  (only relevant once brute-forcing is no longer possible)
- the starting positions are an 8th of the inner n - 2 by n - 2 square minus the middle 1 or 4 vertices for n > 3,
  there are (n / 2 * (n / 2 + 1) / 2 - 1) (floordivs) many of them
- the local_best, global_best conditions are permissive in order to print all optimal solutions,
  the variable names do not reflect this as it was changed later
- vertex_masks is a matrix of bitmasks that are only true for every vertex's surrounding squares,
  it is used as it is faster than calculating the surrounding indices at every step
- n = 8, 9 are good for testing as they run in human time
*/

/*
possible optimizations:
- starting direction pruning on the lines of symmetry
- only checking 5 dirs instead of 8 based on the last dir,
  maybe even less as we only need one bit to be already true as per the zero waste note
  (unlikely due to the above note on vertex_masks)
- better branch prediction (current was tested at n = 8)
- making use of more / better built-ins?

- checking whether or not the remaining 0s are connected (bfs is slower for n < 10 and thus hard to test)
- using a sliding window instead of the entirety of vertex_masks? (unlikely due to the above note)

- using constexpr more?
- better compile flags?
- better max update logic?
- better multithreading?
- better order in which directions are checked??
- better brute-force algorithm??
- better ...

- rewrite to use GPU instead??
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <limits>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <array>
#include <bitset>

constexpr int n = 9;
constexpr int MAX_LEN = 50;

constexpr int total_bits = n * n;
// !!! added_count heuristic depends on the orthogonals to be in the first half
constexpr int dx[8] = {1, 0, -1, 0, 1, -1, 1, -1};
constexpr int dy[8] = {0, 1, 0, -1, 1, 1, -1, -1};

using MaskType = std::array<std::array<std::bitset<total_bits>, n+1>, n+1>;

constexpr MaskType create_vertex_masks() {
    MaskType masks{};
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
    std::array<std::array<bool, n+1>, n+1> is_outer = {};
    for (int i = 0; i <= n; ++i) {
        is_outer[i][0] = true;
        is_outer[i][n] = true;
        is_outer[0][i] = true;
        is_outer[n][i] = true;
    }
    return is_outer;
}
constexpr auto is_outer = create_is_outer();

std::atomic<int> global_best(std::numeric_limits<int>::max());
std::vector<std::pair<int, int>> global_best_path;
std::mutex global_mutex;
/*
bool are_zeros_connected(std::bitset<total_bits>& mask) {
    return false
}
*/
inline void dfs(int x, int y, std::bitset<total_bits>& mask, int len, 
                std::vector<std::pair<int, int>>& path, int& local_best,
                std::vector<std::pair<int, int>>& local_best_path) {
    int effective_best = std::min(local_best, global_best.load(std::memory_order_relaxed));
    const int count = mask.count();
    if (len + (total_bits - count + 2) / 3 > effective_best) [[likely]] return;

    if (count == mask.size()) {
        if (len <= local_best) [[unlikely]] {
            local_best = len;
            local_best_path = path;
            if (len <= global_best.load(std::memory_order_relaxed)) [[unlikely]] {
                std::lock_guard<std::mutex> lock(global_mutex);
                if (len <= global_best) {
                    global_best = len;
                    global_best_path = path;
                    std::cout << "New best: " << len << ", Path: ";
                    for (const auto& p : path) {
                        std::cout << "(" << p.first << "," << p.second << ")";
                    }
                    std::cout << "\n";
                }
            }
        }
        return;
    }

    const auto invmask = ~mask;
    for (int dir = 0; dir < 8; ++dir) {
        const int nx = x + dx[dir];
        const int ny = y + dy[dir];
        if (is_outer[nx][ny]) [[unlikely]] continue;

        const int added_count = (vertex_masks[nx][ny] & invmask).count();
        if (added_count < dir / 4 + 2 && (n != 3 || added_count == 0)) [[likely]] continue;

        /* should be unreachable, useful in case of testing new heuristic
        const int new_len = len + 1;
        if (new_len > MAX_LEN) [[unlikely]] continue;
        */

        path.push_back({nx, ny});
        auto new_mask = mask | vertex_masks[nx][ny];
        dfs(nx, ny, new_mask, len + 1, path, local_best, local_best_path);
        path.pop_back();
    }
}

void search_from(const std::pair<int, int>& start) {
    int local_best = std::numeric_limits<int>::max();
    std::vector<std::pair<int, int>> local_best_path;
    std::vector<std::pair<int, int>> path;
    std::bitset<total_bits> mask = vertex_masks[start.first][start.second];
    path.reserve(MAX_LEN + 1);
    path.push_back(start);
    dfs(start.first, start.second, mask, 0, path, local_best, local_best_path);
}

void run_parallel_search(const std::vector<std::pair<int, int>>& starts) {
    const int hw_concurrency = static_cast<int>(std::thread::hardware_concurrency());
    const int num_threads = starts.empty() ? 0 : std::clamp(hw_concurrency, 1, static_cast<int>(starts.size()));
    std::vector<std::jthread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([i, num_threads, &starts] {
            for (size_t j = i; j < starts.size(); j += num_threads) {
                search_from(starts[j]);
            }
        });
    }
}

int main() {
    constexpr int max_x = (n < 3) ? 1 : n / 2;
    std::vector<std::pair<int, int>> starts;
    for (int x = 1; x <= max_x; ++x) {
        for (int y = 1; y <= x; ++y) {
            if (n < 4 || !(x == max_x && y == max_x))
                starts.emplace_back(x, y);
        }
    }

    run_parallel_search(starts);
    /*
    long long total_runtime = 0;
    int runs = 5;
    for (int i = 0; i < runs; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        run_parallel_search(starts);
    
        auto end_time = std::chrono::high_resolution_clock::now();
        total_runtime += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }

    double average_runtime = static_cast<double>(total_runtime) / runs;
    std::cout << "Average Runtime: " << average_runtime << " ms" << std::endl;
    */
    return 0;
}
