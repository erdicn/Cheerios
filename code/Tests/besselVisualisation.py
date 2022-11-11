from scipy.special import kn
import matplotlib.pyplot as plt
x = np.linspace(0, 10, 10_000_000)
N=1
plt.plot(x, kn(N, x), label='$K_{}(x)$'.format(N))

# for N in range(6):
    # plt.plot(x, kn(N, x), label='$K_{}(x)$'.format(N))
plt.ylim(0, 10)
plt.legend()
plt.title(r'Modified Bessel function of the second kind $K_n(x)$')
plt.show()