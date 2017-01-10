import matplotlib.pyplot as plt
from math import log
import glob

from os.path import dirname, join
from consts import *

folder = OUTPUT_DIR + "/granulometry/"
file_names = glob.glob(folder + "**/*.txt")

inpainted_r = [[] for _ in range(16)]
original_r = [[] for _ in range(16)]

file_names = [n for n in file_names if 'misc' in n]


def scaler(x):
    return log(500 * x + 1)
    # return x


def process(name):
    global inpainted_r, original_r

    target = original_r
    if '_inpainted' in name:
        target = inpainted_r

    with open(name) as f:
        l = [float(a) for a in next(f).split(' ') if a != '' and float(a) != 0]
        target[min(len(l), 15)].append(clean_name(name))
        # if (len(l) < 4 and 'inpainted' in name):
        #     n = name.split('/')
        #     n[3] = 'suspicious'
        #     ext = n[-1].split('.')
        #     ext[-2] += ''
        #     ext[-1] = 'png'
        #     n[-1] = '.'.join(ext)
        #     # print('/'.join(n))
        #     im = cv2.imread('/'.join(n))
        #     cv2.imshow("dd", im)
        #     cv2.waitKey(0)
        #     # print(next(f).split(' '))

    print(clean_name(name))
    color = 'b'
    if '_inpainted' in name:
        color = 'r'
    with open(name) as f:
        # print(next(f).split(' '))
        l = [scaler(float(a)) for a in next(f).split(' ') if a != '']
        # print(l)
        plt.plot(l, color=color)
        # plt.show()


for f_name in file_names:
    if '_inpainted' in f_name:
        continue
        # if 'natur' in f_name or 'misc' in 'f_name' or True:
        #     process(f_name)
        # if 'facade' not in f_name:
    process(f_name)

for f_name in file_names:
    if '_inpainted' in f_name:
        process(f_name)

false_positives = []
true_positives = []
false_negatives = []
true_negatives = []

for l in inpainted_r[0:5]:
    false_negatives.extend(l)

for l in inpainted_r[5:]:
    true_positives.extend(l)

for l in original_r[:5]:
    true_negatives.extend(l)

for l in original_r[5:]:
    false_positives.extend(l)

print("\n\nFalse negatives: %s" % len(false_negatives))
print('\n'.join(false_negatives))

print("\n\nFalse positives: %s" % len(false_positives))
print('\n'.join(false_positives))

print("\n\nTrue positives: %s" % len(true_positives))
print('\n'.join(true_positives))
plt.show()
