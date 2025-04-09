# Minimal path to touch every square in an n by n grid

### Results
`? = unproven`
| n  | Current Best | # Straights | # Diagonals | # Unique Solutions |
|----|--------------|-------------|-------------|--------------------|
| 1  | 0            | 0           | 0           | 1                  |
| 2  | 0            | 0           | 0           | 1                  |
| 3  | 3            | 3           | 0           | 1                  |
| 4  | 6            | 6           | 0           | 1                  |
| 5  | 10           | 9           | 1           | 1                  |
| 6  | 14           | 10          | 4           | 3                  |
| 7  | 20           | 15          | 5           | 6                  |
| 8  | 25           | 15          | 10          | 3                  |
| 9  | 31           | 16          | 15          | 9                  |
| 10 | 39           | 21          | 18          | 34?                |
| 11 | 46?          | 21?         | 25?         | 8?                 |
| 12 | 54?          | 22?         | 32?         | 22?                |
| 13 | 64?          | 27?         | 37?         | 22?                |
| 14 | 73?          | 27?         | 46?         | 6?                 |
| 15 | 83?          | 28?         | 55?         | 20?                |
| 16 | 95?          | 33?         | 62?         |                    |
| 17 | 106?         | 33?         | 73?         | 6?                 |
| 18 | 118?         | 34?         | 84?         | 6?                 |

---

### Running
- Close everything else that would waste CPU resources
- Set `n`
- Set `MAX_LEN` to the `Current Best`
  - Set it to `Current Best - 1` if you only wish to prove that the current best is optimal
```ps
g++ -std=c++2b -Ofast -march=native -flto path.cpp -o path
```

---

### Notes

- This code only tests `(1, 1)` and `(1, 0)` moves
- Chebysev distance is used as $\frac{3}{1} > \frac{2}{1}$, just like $\frac{3}{\sqrt2} > \frac{2}{1}$ and it is faster
- All moves of optimal solutions for 3 < n < 8 are zero waste, this is enforced as a general rule, see `added_count`
- The starting positions are an 8th of the inner $(n - 2)(n - 2)$ square minus the middle 1 or 4 vertices for `n > 3`
  - there are $\frac{n\left(\frac{n}{2}+1\right)}{2} - 1$ (floordivs) many of them
- `n = 8, 9` are good for testing as they run in human time

---

### Possible Optimizations

- starting direction pruning on the lines of symmetry
- only checking 5 `dir`s instead of 8 based on the last `dir`
  - unlikely due to the above note on vertex_masks
- better branch prediction
  - current was tested at `n = 8`
- making use of more / better built-ins?
  <br><br>
- checking whether or not the remaining 0s are connected
  - bfs is slower for `n < 10` and thus hard to test
- using a sliding window instead of the entirety of `vertex_masks`?
  - unlikely due to the above note
  <br><br>
- using `constexpr` more?
- better compile flags?
- better max update logic?
- better multithreading?
- better order in which directions are checked??
- better brute-force algorithm??
- better ...
  <br><br>
- rewrite to use GPU instead??
