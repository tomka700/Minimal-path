# Minimal path to touch every square in an n by n grid

### Results
`? = unproven`
| n  | Current Best | # Straights | # Diagonals | # Unique Solutions |
|----|--------------|-------------|-------------|--------------------|
| 0  | 0            | 0           | 0           | 1                  |
| 1  | 0            | 0           | 0           | 1                  |
| 2  | 0            | 0           | 0           | 1                  |
| 3  | 3            | 3           | 0           | 1                  |
| 4  | 6            | 6           | 0           | 1                  |
| 5  | 10           | 9           | 1           | 1                  |
| 6  | 14           | 10          | 4           | 3                  |
| 7  | 20           | 15          | 5           | 6                  |
| 8  | 25           | 15          | 10          | 3                  |
| 9  | 31           | 16          | 15          | 9                  |
| 10 | 39           | 21          | 18          | 42                 |
| 11 | 46?          | 21?         | 25?         | 8?                 |
| 12 | 54?          | 22?         | 32?         | 47?                |
| 13 | 64?          | 27?         | 37?         | 22?                |
| 14 | 73?          | 27?         | 46?         | 8?                 |
| 15 | 83?          | 28?         | 55?         | 20?                |
| 16 | 95?          | 33?         | 62?         | 19?                |
| 17 | 106?         | 33?         | 73?         | 8?                 |
| 18 | 118?         | 34?         | 84?         | 6?                 |

---

### Running
- Set `n`
  - Close everything else that would waste CPU resources if `n > 8`
- Set `ONLY_PROVE_LENGTH` to `true` if you only wish to prove that the `Current Best` is optimal
  - This option is exponentially faster, but it probably won't print any paths
```ps
g++ -std=c++2b -Ofast -march=native -flto path.cpp -o path
```
**Disclaimer:** `n = 10` takes days to run, anything beyond that has yet to produce an optimal output

---

### Notes

- This program only tests `(1, 1)` and `(1, 0)` moves
- Chebysev distance is used as $\frac{3}{1} > \frac{2}{1}$, just like $\frac{3}{\sqrt2} > \frac{2}{1}$ and it is faster
- All moves of optimal solutions for `3 < n < 8` are zero waste, this is enforced as a general rule, see `added.count()`
- The starting positions are an 8th of the inner $(n - 2)(n - 2)$ square minus the middle 1 or 4 vertices for `n > 3`
  - there are $\frac{n\left(\frac{n}{2}+1\right)}{2} - 1$ (floordivs) many of them
- `n = 8, 9` are good for testing as they run in human time

---

### Todo
- rename `current_best` to disambiguate
- prune starting dirs by symmetry
- constexpr calculate all forced starting path sections and start threads from there
- explain obvious moves in `force_obvious_moves`
- savestate and continue?
- early exit in case of too large `n`

---

### Possible Optimizations

- better branch prediction
  - current was tested at `n = 8`
- making use of more / better built-ins?
  <br><br>
- checking whether or not the remaining 0s are connected
  - bfs is slower for `n < 10` and thus hard to test
- only checking 5 `dir`s instead of 8 based on the last `dir`
  - unlikely due to coordinate calculations being slow
- using a sliding window instead of the entirety of `vertex_masks`?
  - unlikely due to coordinate calculations being slow
  <br><br>
- using `constexpr` more?
- better compile flags?
- better max update logic?
- better multithreading?
- better ...
  <br><br>
- rewrite to use GPU instead??
