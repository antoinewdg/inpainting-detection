from consts import *

file_names = glob.glob(OUTPUT_DIR + "/hist_all/**/*.txt")
file_names.sort()

# file_names = [n for n in file_names if 'misc' in n]


# file_names = [n for n in file_names if 'ny' in n]

file_names = [n for n in file_names if 'facade' not in n]
file_names = [clean_name(n) for n in file_names]
medians_true = []
medians_false = []


def plt_hist(name, dir, c, bins):
    if not os.path.isfile(get_full_name(name, dir, 'txt')):
        return
    data = np.loadtxt(get_full_name(name, dir, 'txt'))

    # data = data[data <= 20000]
    if len(data) == 0:
        return
    # print(max(data))
    # print(bins)
    hist, bins = np.histogram(data, bins=bins, density=True)
    x = [(b + bins[i + 1]) // 2 for i, b in enumerate(bins[:-1])]
    m = np.max(hist)
    i = len(x)
    while hist[i - 1] <= 0.05 * m:
        i -= 1

    hist = hist[:i]
    hist /= np.sum(hist)

    # x = [a for i, a in enumerate(x) if hist[i] >= 0.05 * m]
    plt.bar(x[:i], hist, width=x[1] - x[0])
    return bins


def get_noise(name):
    filename = get_full_name(name, "noise", "txt")
    with open(filename) as f:
        return float(next(f))


for name in file_names:
    # print(get_full_name(name, 'hist_symmetric', 'txt'))
    sigma = get_noise(name)
    print(name, sigma)
    if sigma == 0:
        continue
    expected = np.sqrt(2 * (sigma ** 2) * np.random.chisquare(75, 10000))

    hist, bins = np.histogram(expected, bins=100, density=True)
    x = [(b + bins[i + 1]) / 2 for i, b in enumerate(bins[:-1])]
    plt_hist(name, 'hist_symmetric', 'auto', 'auto')
    plt.show()
    plt.bar(x, hist, width=x[1] - x[0])
    plt.show()
    # print(clean_name(name))
    # # reg_name = name.split('/')
    # # reg_name[3] = 'hist_neg'
    # # reg_name = '/'.join(reg_name)
    # # reg = np.loadtxt(reg_name)
    #
    # bins = plt_hist(name, 'r', 'auto')
    # # plt_hist(reg_name, 'b', bins)
    # if bins is not None:
    #     plt.show()

# file_names = [OUTPUT_DIR + '/variance/misc/bus_sky.png.txt']
