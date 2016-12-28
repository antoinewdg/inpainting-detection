from consts import *

y = np.loadtxt(OUTPUT_DIR + '/noise_levels.txt')
x = [2.5 * (i + 1) for i in range(len(y))]
print(x)

print([b / a for a, b in zip(x, y)])

plt.plot(x, y)
plt.show()
