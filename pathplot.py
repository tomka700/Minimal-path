import matplotlib.pyplot as plt
# paste output into path list and replace all ") (" with "), ("
path = [
    (1,1), (1,2), (1,3), (2,4), (1,5), (1,6), (2,6), (3,6), (4,5), (5,6), (6,6), (6,5), (6,4), (6,3), (6,2), (6,1), (5,1), (4,1), (3,1), (3,2), (4,3)
]

x_coords, y_coords = zip(*path)

plt.figure(figsize=(6,6))
plt.plot(x_coords, y_coords, marker='o', linestyle='-', color='b', markersize=5)

for i, (x, y) in enumerate(path):
    plt.text(x, y, f"({x},{y})", fontsize=9)

plt.grid(True)
plt.xticks(range(min(x_coords)-1, max(x_coords)+2))
plt.yticks(range(min(y_coords)-1, max(y_coords)+2))

plt.show()
