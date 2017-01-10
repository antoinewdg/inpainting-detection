from consts import *

file_names = glob.glob(OUTPUT_DIR + "/hist_pos/**/*.txt")
file_names.sort()

file_names = [n for n in file_names if 'misc' in n]

file_names = [n for n in file_names if 'ny' in n]


def plt_hist(name, c, bins):
    data = np.loadtxt(name)

    # data = data[data <= 20000]
    data = np.sqrt(data)
    print(max(data))
    # print(bins)
    hist, bins = np.histogram(data, bins=bins, density=True)
    x = [(b + bins[i + 1]) // 2 for i, b in enumerate(bins[:-1])]
    plt.plot(x, hist, c=c)
    return bins


for name in file_names:
    print(clean_name(name))
    reg_name = name.split('/')
    reg_name[3] = 'hist_neg'
    reg_name = '/'.join(reg_name)
    reg = np.loadtxt(reg_name)
    fig = plt.figure()
    fig.canvas.set_window_title(name)
    bins = plt_hist(name, 'r', 'auto')
    plt_hist(reg_name, 'b', bins)
    plt.show()

# file_names = [OUTPUT_DIR + '/variance/misc/bus_sky.png.txt']
