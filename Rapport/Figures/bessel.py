from scipy import special
import matplotlib as mpl
from matplotlib import pyplot as plt
import numpy as np

X = []
X1 = []
T = np.linspace(0,5,1000000)

ax = plt.subplot() 
ax.plot(T, special.k1(T), color ='r', label='K1',)
ax.plot(T,special.k0(T), color='b', label='K0')
ax.set_ybound(0, 5)
ax.set_xbound(0, 5)

ax.legend()
plt.savefig("Besselk1_K0.pdf", dpi=600)
#plt.show()