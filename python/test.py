import glob

from consts import *

# file_names = glob.glob(OUTPUT_DIR + "/variance/**/*.txt")
# file_names.sort()
file_names = [OUTPUT_DIR + '/variance/misc/bus_sky.png.txt']

for name in file_names:
    print(name)
    distances = np.loadtxt(name, dtype=float)[2:-2, 2:-2]
    # distances = distances[distances <= 20.0]
    print(np.min(distances))
    # distances = np.log(1 + distances)
    hist, edges = np.histogram(distances, bins='auto')
    plt.plot(edges[:-1], hist)
    plt.show()
    # hist = np.histogram(out, bins='auto')
