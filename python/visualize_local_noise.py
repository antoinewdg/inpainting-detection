from consts import *

data = np.loadtxt(OUTPUT_DIR + "/noise_hist.txt")
print("Global noise level=%s" % data[0])
plt.hist(data[1:], bins=20)
plt.show()
