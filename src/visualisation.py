from matplotlib import pyplot as plt
import matplotlib.animation as animation
import numpy as np
from matplotlib.patches import Circle

fichier_donnees_init = "donnees_initiales.txt"
fichier_donnees      = "donnees.txt"
#lis le fichier de donnees initiales pour prendre le dt
donnees_initiales = list(map(str, open(fichier_donnees_init, 'rt').readlines()))
donnees_initiales_temps = [i for i in donnees_initiales[1].split()]
nb_cheerios = int  (donnees_initiales_temps[0])
NT          = int  (donnees_initiales_temps[1])
dt          = float(donnees_initiales_temps[2])

donnees_bord = [float(i) for i in donnees_initiales[3].split()]
rayon_bord =  donnees_bord[0]
bord_centre = (donnees_bord[1], donnees_bord[2])   


#lis le fichier colonne par colonne 
Donnees = np.loadtxt(open(fichier_donnees, 'rt').readlines())
#Donnees = np.loadtxt(open("Experience1.dat", 'rt').readlines())
T   = Donnees[:, 0]
X   = Donnees[:, 1]
Y   = Donnees[:, 2]
D   = Donnees[:, 3]
# V_x = Donnees[:, 4]
# V_y = Donnees[:, 5]
# A_x = Donnees[:, 6]
# A_y = Donnees[:, 7]
# F_x = Donnees[:, 8]
# F_y = Donnees[:, 9]
# M   = Donnees[:, 10]
# ^ Si on decide de inclur plus de donnees dans le donnees.txt

patches = []
for p in range(nb_cheerios):
    patches.append(Circle((X[0+p],Y[0+p]), radius=D[0+p]/2, color = "y" #fc='b')
                                ))
patches.append(Circle( bord_centre, rayon_bord, color = "k", alpha = 0.1))

fig, ax = plt.subplots()
ax.set_aspect('equal', 'box')
ax.set_xlim([bord_centre[0]-1.1*rayon_bord, bord_centre[0]+1.1*rayon_bord])
ax.set_ylim([bord_centre[1]-1.1*rayon_bord, bord_centre[1]+1.1*rayon_bord])
ax.set_title("Positions des cheerios au cours du temps")
ax.set_xlabel("m")
ax.set_ylabel("m")

def init():
    for i in range(len(patches)-1):
        patches[i].center = (X[0+i],Y[0+i])
        ax.add_patch(patches[i])
    patches[-1].center = bord_centre
    ax.add_patch(patches[-1])
    return patches

time_template = 'time = %.2fs'
time_text = ax.text(0.05, 0.9, '', transform=ax.transAxes)

def animate(i):
    if i + len(patches) < len(X):
        for p in range(len(patches)-1):
            patches[p].center = (X[p+i], Y[p+i])
            patches[p].radius = D[p+i]/2
        time_text.set_text(time_template % (T[i]*dt))
    return patches
augmentation = int(1/(dt*100))                                       #[i for i in range(0,NT, 10)] ou NT
if augmentation == 0: augmentation = 1
anim = animation.FuncAnimation(fig, animate, init_func=init, frames= [i for i in range(0, len(T) , augmentation)]
                               , interval=1, blit=False#, repeat = True 
                               )
#anim.save('visualisation.gif', fps=100, dpi=200)
plt.show()