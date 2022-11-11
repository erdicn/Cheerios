from matplotlib import pyplot as plt
from celluloid import Camera
from matplotlib.animation import FuncAnimation
import matplotlib.animation as animation
import numpy as np
from matplotlib.patches import Circle
from itertools import combinations

Donnees = np.loadtxt(open("./5_6code/VisualisationTest/donnees_copy.txt", 'rt').readlines())
T = Donnees[:, 0]
X = Donnees[:, 1]
Y = Donnees[:, 2]
# D = Donnees[:, 3]
# V_x = Donnees[:, 4]
# V_y = Donnees[:, 5]
# A_x = Donnees[:, 6]
# A_y = Donnees[:, 7]
# F_x = Donnees[:, 8]
# F_y = Donnees[:, 9]
# M = Donnees[:, 10]

X_a = []
Y_a = []
X_b = []
Y_b = []

print(len(T), len(X), len(Y))

i = 0
while i < len(T):
    X_a.append(X[i])
    Y_a.append(Y[i])
    X_b.append(X[i+1])
    Y_b.append(Y[i+1])
    i=i+2

nb_cheerios = len([i for i in T if i == 0])
print(nb_cheerios)
NT = int(max(T) + 1)

vmin = min(min(X[0:nb_cheerios]),min(Y[0:nb_cheerios]))
vmax = max(max(X[0:nb_cheerios]),max(Y[0:nb_cheerios]))

radius = 0.003/2.
styles = {'edgecolor': 'C0', 'linewidth': 2, 'fill': None}
# circles = []
# for i in range(0, len(X)):
#     circle = Circle(xy=(X[i],Y[i]), radius=radius, color='red')
#     circles.append(circle)
# # print(circles)

patch1 = Circle((X_a[0],Y_a[0]), radius=radius, fc='y')
patch2 = Circle((X_b[0],Y_b[0]), radius=radius, fc='y')

fig, ax = plt.subplots()
for s in ['top','bottom','left','right']:
    ax.spines[s].set_linewidth(2)
    ax.set_aspect('equal', 'box')
    ax.set_xlim([vmin - 0.05, vmax + 0.05])
    ax.set_ylim([vmin - 0.05, vmax + 0.05])
    ax.xaxis.set_ticks([])
    ax.yaxis.set_ticks([])

def init():
    patch1.center = (X_a[0],Y_a[0])
    patch2.center = (X_b[0],Y_b[0])
    ax.add_patch(patch1)
    ax.add_patch(patch2)
    return patch1, patch2,

def animate(i):
    x_a, y_a = patch1.center
    x_b, y_b = patch2.center

    x_a = X_a[i]
    y_a = Y_a[i]
    x_b = X_b[i]
    y_b = Y_b[i]

    patch1.center = (x_a, y_a)
    patch2.center = (x_b, y_b)

    return patch1, patch2,

anim = animation.FuncAnimation(fig, animate, init_func=init, frames=NT, interval=1, blit=True)
anim.save('./5_6code/VisualisationTest/donnees_visualisation_matplot.gif', fps=400, dpi=200)
# plt.show()