# Minimal path to touch every square in an n by n grid

### Results
`? = unproven`
| n  | Length | # Orthogonals | # Diagonals | # Unique Solutions | # Solutions |
|----|--------|---------------|-------------|--------------------|-------------|
| 0  | 0      | 0             | 0           | 1                  | 1           |
| 1  | 0      | 0             | 0           | 1                  | infinite    |
| 2  | 0      | 0             | 0           | 1                  | 1           |
| 3  | 3      | 3             | 0           | 1                  | 4           |
| 4  | 6      | 6             | 0           | 1                  | 4           |
| 5  | 10     | 9             | 1           | 1                  | 8           |
| 6  | 14     | 10            | 4           | 3                  | 16          |
| 7  | 20     | 15            | 5           | 6                  | 48          |
| 8  | 25     | 15            | 10          | 3                  | 24          |
| 9  | 31     | 16            | 15          | 9                  | 64          |
| 10 | 39     | 21            | 18          | 42                 | 336         |
| 11 | 46?    | 21?           | 25?         | 8?                 | 64?         |
| 12 | 54?    | 22?           | 32?         | 47?                |             |
| 13 | 64?    | 27?           | 37?         | 22+?               |             |
| 14 | 73?    | 27?           | 46?         | 8?                 | 64?         |
| 15 | 83?    | 28?           | 55?         | 20+?               |             |
| 16 | 95?    | 33?           | 62?         | 19+?               |             |
| 17 | 106?   | 33?           | 73?         | 8?                 | 64?         |
| 18 | 118?   | 34?           | 84?         | 6+?                |             |

---

### Running
- Set `n`
  - Close everything else that would waste CPU resources for `n > 9`
- Set `ONLY_PROVE_LENGTH` to `true` if you only wish to prove that the current best `Length` is optimal
  - This option is exponentially faster, but it probably won't print any paths
```ps
g++ -std=c++2b -O3 -march=native -flto path.cpp -o path
```
**Disclaimer:** `n = 10` takes days to run, anything beyond that has yet to produce an optimal output

---

### Notes
- Chebyshev distance is used as it is equivalent in the case of only using (1,1) and (1,0) moves and it is faster
  - `n = 3` is the only special case, where false positives would arise, they are filtered out manually
  - Accoordingly, this program only tests `(1, 1)` and `(1, 0)` moves
- The formulas used for `CURRENT_BEST` in the code are explained here: https://math.stackexchange.com/q/5036847
- All moves of optimal solutions for `3 < n < 8` are zero waste, this is enforced as a general rule, see `added.count()`
- The starting positions are an 8th of the inner $(n - 2)(n - 2)$ square minus the middle 1 or 4 vertices for `n > 3`
  - there are $\frac{n\left(\frac{n}{2}+1\right)}{2} - 1$ (floordivs) many of them
  - `n < 11` isn't brute-forced for all of these, change the last `return` condition in `force_obvious_moves` for testing
- `n = 8, 9` are good for testing as they run in human time

---

### Todo
- add brute-force option for already proven `n`
- use arrays where possible
- reuse smaller solutions for the same `n mod 3`
- prune starting dirs by symmetry
- explain obvious moves in `force_obvious_moves`
- savestate and continue?
- early exit in case of too large `n`

---

### Possible Optimizations
- making use of more / better built-ins?
  <br><br>
- checking whether or not the remaining 0s are connected
  - bfs is slower for `n < 10` and thus hard to test
- only checking 5 `dir`s instead of 8 based on the last `dir`
  - unlikely due to coordinate calculations being slow
- using a sliding window instead of the entirety of `VERTEX_MASKS`?
  - unlikely due to coordinate calculations being slow
  <br><br>
- using `constexpr` more?
- better max update logic?
- better multithreading?
- better ...
  <br><br>
- rewrite to use GPU instead??
