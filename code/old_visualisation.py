"""@file visualisation.py
@authors Bapiste BRAUN-DELVOYE, Erdi Çan
@brief FIchier de visualisation de notre simulation enutilisant matplotlib
@version 5.5
@date 2022-11-08
  
@copyright Copyright (c) 2022
"""
from matplotlib import pyplot as plt
from celluloid import Camera
from matplotlib.animation import FuncAnimation
import matplotlib.animation as animation
import numpy as np

# lecture de donnee
Donnees = np.loadtxt(open("donnees.txt", 'rt').readlines())
T = Donnees[:, 0]
X = Donnees[:, 1]
Y = Donnees[:, 2]
D = Donnees[:, 3]
V_x = Donnees[:, 4]
V_y = Donnees[:, 5]
A_x = Donnees[:, 6]
A_y = Donnees[:, 7]
F_x = Donnees[:, 8]
F_y = Donnees[:, 9]
M = Donnees[:, 10]

# on compte combien de 0 il ya et ca ca nous donne le nombre le objets dans un pas de temps
nb_cheerios = len([i for i in T if i == 0])
print(nb_cheerios)
NT = int(max(T) + 1)

vmin = min(min(X[0:nb_cheerios]),min(Y[0:nb_cheerios]))
vmax = max(max(X[0:nb_cheerios]),max(Y[0:nb_cheerios]))

fig, ax = plt.subplots(figsize=(10,10),dpi = 72)
ax.set_xlim([vmin - 0.05, vmax + 0.05])
ax.set_ylim([vmin - 0.05, vmax + 0.05])

scat = ax.scatter(X[0:nb_cheerios], Y[0:nb_cheerios])
#x = np.linspace(0, 10)
s = ((ax.get_window_extent().width  / (vmax-vmin+1.) * 72./fig.dpi) ** 2)/1200#**2
#s = ((ax.get_window_extent().width  / (vmax-vmin+1.) * 72./fig.dpi) ** 2) / 16
L_s = [s for _ in range(nb_cheerios)]
def animate(i):
    points = []
    points = [(X[c], Y[c]) for c in range(i, i+nb_cheerios)]
    # print(points)
    scat.set_offsets(points)
    scat.set_sizes(L_s) #D[i:i+nb_cheerios]*10000
    #scat.set_color(["c", "b"])
    return scat,


ani = animation.FuncAnimation(fig, animate,  # repeat=True,
                              frames=NT  # [nt for nt in range(NT)]
                              , interval=0.01)
plt.show()

# fig, ax = plt.subplots()
# xdata, ydata = [], []
# ln, = ax.plot([], [], 'ro')

# def init():
#     ax.set_xlim(min(X), max(X))
#     ax.set_ylim(min(Y), max(Y))
#     return ln,

# def update(frame):
#     # xdata.append(frame)
#     # ydata.append(np.sin(frame))
#     i = frame
#     print("frame ", i , end="\r")
#     xdata = X[i:i+nb_cheerios]
#     ydata = Y[i:i+nb_cheerios]
#     ln.set_data(xdata, ydata)
#     return ln,

# ani = FuncAnimation(fig, update, frames=[nt for nt in range(NT)],interval = 10, #init_func=init,  # a activer apres que on a fais les cas de collisions
#                         blit=True)
# plt.show()
