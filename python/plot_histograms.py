from consts import *

file_names = glob.glob(OUTPUT_DIR + "/hist_pos/**/*.txt")
file_names.sort()

file_names = [n for n in file_names if 'misc' in n]


def plt_hist(name):
    data = np.loadtxt(name)
    data = data[data <= 20000]
    hist, bins = np.histogram(data, bins='auto', density=True)
    x = [(b + bins[i + 1]) // 2 for i, b in enumerate(bins[:-1])]
    plt.plot(x, hist)


for name in file_names:
    print(clean_name(name))
    reg_name = name.split('/')
    reg_name[3] = 'hist_neg'
    reg_name = '/'.join(reg_name)
    reg = np.loadtxt(reg_name)
    plt_hist(name)
    plt_hist(reg_name)
    plt.show()

# file_names = [OUTPUT_DIR + '/variance/misc/bus_sky.png.txt']
