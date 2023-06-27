import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

fig = plt.figure()
ax = fig.gca()

colors = {0 : "ro", 1 : "go", 2 : "bo", 3 : "co", 4 : "mo", 5 : "yo"}
cntColors = len(colors)
x = []
y = []
cl = []

with open ("clusters.txt", "r") as f:
    for line in f.readlines():
        crd = line.split()
        x.append(float(crd[0]))
        y.append(float(crd[1]))
        cl.append(int(crd[2]))

print(max(cl))

for i in range(len(cl)):
    if (cl[i] == -1):
        ax.plot(x[i], y[i], "ko")
    else:
        ax.plot(x[i], y[i], colors[cl[i] % cntColors])

plt.xticks(np.arange(min(x), max(x)+1, 10 / sqrt(2)))
plt.yticks(np.arange(min(y), max(y)+1, 10 / sqrt(2)))
plt.grid()
plt.show()