import matplotlib.pyplot as plt
import re

# paste output into data string block, equivalent paths will be filtered out
data = """
New best: 20, Path: (2,1)(1,1)(1,2)(1,3)(2,3)(3,3)(4,2)(4,1)(5,1)(6,1)(6,2)(6,3)(5,4)(6,5)(6,6)(5,6)(4,6)(3,5)(2,6)(1,6)(1,5)
New best: 20, Path: (1,1)(2,1)(3,1)(4,1)(4,2)(3,3)(2,3)(1,3)(1,4)(1,5)(1,6)(2,6)(3,5)(4,6)(5,6)(6,6)(6,5)(5,4)(6,3)(6,2)(6,1)
New best: 20, Path: (2,1)(1,1)(1,2)(1,3)(1,4)(1,5)(1,6)(2,6)(3,5)(4,6)(5,6)(6,6)(6,5)(5,4)(6,3)(6,2)(6,1)(5,1)(4,1)(4,2)(3,3)
New best: 20, Path: (1,1)(2,1)(3,1)(3,2)(3,3)(2,3)(1,3)(1,4)(1,5)(1,6)(2,6)(3,5)(4,6)(5,6)(6,6)(6,5)(5,4)(6,3)(5,2)(5,1)(6,1)
New best: 20, Path: (2,1)(1,1)(1,2)(2,3)(1,4)(1,5)(1,6)(2,6)(3,5)(4,6)(5,6)(6,6)(6,5)(6,4)(5,4)(4,3)(4,2)(4,1)(5,1)(6,1)(6,2)
New best: 20, Path: (1,1)(2,1)(3,1)(4,2)(5,1)(6,1)(6,2)(6,3)(5,4)(6,5)(6,6)(5,6)(4,6)(3,6)(3,5)(3,4)(2,3)(1,3)(1,4)(1,5)(1,6)
New best: 20, Path: (1,1)(2,1)(3,1)(4,2)(5,1)(6,1)(6,2)(6,3)(5,4)(6,5)(6,6)(5,6)(4,6)(3,6)(2,6)(1,6)(1,5)(1,4)(1,3)(2,3)(3,4)
New best: 20, Path: (1,1)(2,1)(2,2)(1,3)(2,4)(1,5)(1,6)(2,6)(3,6)(4,5)(5,6)(6,6)(6,5)(6,4)(6,3)(6,2)(6,1)(5,1)(4,1)(4,2)(4,3)
New best: 20, Path: (1,1)(2,1)(2,2)(1,3)(2,4)(1,5)(1,6)(2,6)(3,6)(4,5)(5,6)(6,6)(6,5)(6,4)(6,3)(5,3)(4,3)(4,2)(4,1)(5,1)(6,1)
New best: 20, Path: (1,1)(1,2)(2,2)(3,1)(4,2)(5,1)(6,1)(6,2)(6,3)(5,4)(6,5)(6,6)(5,6)(4,6)(3,6)(3,5)(3,4)(2,4)(1,4)(1,5)(1,6)
New best: 20, Path: (1,1)(1,2)(2,2)(3,1)(4,2)(5,1)(6,1)(6,2)(6,3)(5,4)(6,5)(6,6)(5,6)(4,6)(3,6)(2,6)(1,6)(1,5)(1,4)(2,4)(3,4)
New best: 20, Path: (1,1)(1,2)(1,3)(2,3)(3,3)(3,2)(3,1)(4,1)(5,1)(6,1)(6,2)(5,3)(6,4)(6,5)(6,6)(5,6)(4,5)(3,6)(2,5)(1,5)(1,6)
New best: 20, Path: (1,1)(1,2)(1,3)(1,4)(2,4)(3,3)(3,2)(3,1)(4,1)(5,1)(6,1)(6,2)(5,3)(6,4)(6,5)(6,6)(5,6)(4,5)(3,6)(2,6)(1,6)
New best: 20, Path: (1,1)(1,2)(1,3)(2,4)(1,5)(1,6)(2,6)(3,6)(4,5)(5,6)(6,6)(6,5)(6,4)(6,3)(6,2)(6,1)(5,1)(4,1)(3,1)(3,2)(4,3)
New best: 20, Path: (1,1)(1,2)(1,3)(2,4)(1,5)(1,6)(2,6)(3,6)(4,5)(5,6)(6,6)(6,5)(6,4)(6,3)(5,3)(4,3)(3,2)(3,1)(4,1)(5,1)(6,1)
"""

def all_symmetries(path, grid_size):
    rotations = [
        lambda p: p,
        lambda p: [(y, grid_size + 1 - x) for x, y in p],
        lambda p: [(grid_size + 1 - x, grid_size + 1 - y) for x, y in p],
        lambda p: [(grid_size + 1 - y, x) for x, y in p]
    ]
    
    mirror = lambda p: [(y, x) for x, y in p]
    
    return {
        tuple(transformation)
        for rot in rotations
        for transformation in (
            rot(path),
            mirror(rot(path)),
            list(reversed(rot(path))),
            list(reversed(mirror(rot(path))))
        )
    }

def filter_paths(paths):
    grid_size = max(max(x, y) for path in paths for (x, y) in path)
    seen = set()
    unique_paths = []
    for path in paths:
        sym_set = all_symmetries(path, grid_size)
        if sym_set & seen:
            continue
        unique_paths.append(path)
        seen.update(sym_set)
        
    return unique_paths

lines = data.strip().split('\n')
paths = []
for line in lines:
        path = [(int(x), int(y)) for x, y in re.findall(r"\((\d+),(\d+)\)", line)]
        paths.append(path)

paths = filter_paths(paths)

for i, path in enumerate(paths):
    x_coords, y_coords = zip(*path)
    
    plt.figure(figsize=(6, 6))
    plt.plot(x_coords, y_coords, marker='o', linestyle='-', color='b', markersize=5)
    
    for x, y in path:
        plt.text(x, y, f"({x},{y})", fontsize=9)
    
    plt.grid(True)
    plt.title(f"Path {i+1}")
    plt.xticks(range(min(x_coords)-1, max(x_coords)+2))
    plt.yticks(range(min(y_coords)-1, max(y_coords)+2))
    
    plt.show()
