from consts import *

file_names = glob.glob(OUTPUT_DIR + "/hist_pos/**/*.txt")
file_names.sort(key=lambda x: ('facade' in x, x))

# file_names = [n for n in file_names if 'misc' in n]


# file_names = [n for n in file_names if 'ny' in n]


deciles_true = [[] for _ in range(9)]
deciles_false = [[] for _ in range(9)]


def compute_deciles(data, deciles):
    for i in range(9):
        deciles[i].append(np.percentile(data, 10 * (i + 1)))
        print(deciles[i][-1], end=' ')
    print()


def process(name):
    data = np.loadtxt(name)
    if len(data) == 0:
        return

    print(clean_name(name))
    data = np.sqrt(data)

    if 'inpainted' in name:
        compute_deciles(data, deciles_true)
    else:
        compute_deciles(data, deciles_false)


def plot(a, color):
    hist, bin_edges = np.histogram(a, bins='auto')
    x = [(bin_edges[i] + bin_edges[i + 1]) // 2 for i in range(len(bin_edges) - 1)]
    # plt.hist(medians_false, bins=100)
    plt.plot(x, hist, c=color)


for name in file_names[:200]:
    process(name)

for i in range(9):
    print("%s-th decile" % (i + 1))
    plt.figure()
    plot(deciles_true[i], "green")
    plot(deciles_false[i], "red")
    plt.show()
