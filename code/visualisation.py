from matplotlib import pyplot as plt
import matplotlib.animation as animation
import numpy as np
from matplotlib.patches import Circle

Donnees = np.loadtxt(open("donnees.txt", 'rt').readlines())
T   = Donnees[:, 0]
X   = Donnees[:, 1]
Y   = Donnees[:, 2]
D   = Donnees[:, 3]
V_x = Donnees[:, 4]
V_y = Donnees[:, 5]
A_x = Donnees[:, 6]
A_y = Donnees[:, 7]
F_x = Donnees[:, 8]
F_y = Donnees[:, 9]
M   = Donnees[:, 10]

nb_cheerios = len([i for i in T if i == 0])
print(nb_cheerios)
NT = int(max(T) + 1)

vmin = min(min(X[0:nb_cheerios]),min(Y[0:nb_cheerios]))
vmax = max(max(X[0:nb_cheerios]),max(Y[0:nb_cheerios]))

# radius = 0.003/2.
styles = {'edgecolor': 'C0', 'linewidth': 2, 'fill': None}
# circles = []
# for i in range(0, len(X)):
#     circle = Circle(xy=(X[i],Y[i]), radius=radius, color='red')
#     circles.append(circle)
# # print(circles)

patches = []
for p in range(nb_cheerios):
    patches.append(Circle((X[0+p],Y[0+p]), radius=D[0+p]/2, color = "y" #fc='b')
                                ))

# patch1 = Circle((X_a[0],Y_a[0]), radius=radius, fc='y')
# patch2 = Circle((X_b[0],Y_b[0]), radius=radius, fc='y')

fig, ax = plt.subplots()
for s in ['top','bottom','left','right']:
    # ax.spines[s].set_linewidth(2)
    ax.set_aspect('equal', 'box')
    # ax.xaxis.set_ticks([])
    # ax.yaxis.set_ticks([])
    ax.set_xlim([vmin - 0.05, vmax + 0.05])
    ax.set_ylim([vmin - 0.05, vmax + 0.05])

def init():
    for i in range(len(patches)):
        patches[i].center = (X[0+i],Y[0+i])
        ax.add_patch(patches[i])
    return patches

def animate(i):
    for p in range(len(patches)):
        patches[p].center = (X[p+i], Y[p+i])
        patches[p].radius = D[p+i]/2
        
    return patches
                                                                #[i for i in range(0,NT, 10)] ou NT
anim = animation.FuncAnimation(fig, animate, init_func=init, frames=[i for i in range(0,NT, 10)]
                               , interval=0.1, blit=True#, repeat = True 
                               )
#anim.save('./5_6code/VisualisationTest/donnees_visualisation_matplot.gif', fps=400, dpi=200)
plt.show()