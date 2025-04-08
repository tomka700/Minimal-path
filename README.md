# Minimal path to touch every square in an n by n grid

### Results
`? = unproven`
| # Straights | # Diagonals | # Unique Optimal Solutions |
|-------------|-------------|-----------------------------|
| 3           | 0           | 1                           |
| 6           | 0           | 1                           |
| 9           | 1           | 1                           |
| 10          | 4           | 3                           |
| 15          | 5           | 6                           |
| 15          | 10          | 3                           |
| 16          | 15          | 9                           |
| 21          | 18          | 18?                         |
| 21          | 25?         | 8?                          |
| 22          | 32?         | 22?                         |
| 27          | 37?         | 22?                         |
| 27          | 46?         | 6?                          |
| 28          | 55?         | 20?                         |
| 33          | 62?         |                             |
| 33          | 73?         | 6?                          |
| 34          | 84?         | 6?                          |

---

### Running
- Close everything else that would waste CPU resources
- Set `n`
- Set `MAX_LEN` to the current best (sum of the first two numbers in the above table)
  - Set it to the current best - 1 if you only wish to prove that the current best is optimal
```ps
g++ -std=c++2b -Ofast -march=native -flto path.cpp -o path
```

---

### Notes

- This code only tests `(1, 1)` and `(1, 0)` moves
- Chebysev distance is used as $\frac{3}{1} > \frac{2}{1}$, just like $\frac{3}{\sqrt2} > \frac{2}{1}$ and it is faster
- All moves of optimal solutions for 3 < n < 8 are zero waste, this is enforced as a general rule, see `added_count`
- The starting positions are an 8th of the inner $(n - 2)(n - 2)$ square minus the middle 1 or 4 vertices for $n > 3$
  - there are $\frac{n\left(\frac{n}{2}+1\right)}{2} - 1$ (floordivs) many of them
- The `global_best` condition is permissive in order to print all optimal solutions
- `vertex_masks` is a matrix of bitmasks that are only true for every vertex's surrounding squares,  
 it is used as it is faster than calculating the surrounding indices at every step
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
