import matplotlib.pyplot as plt
from math import log
import glob

from os.path import dirname, join
from consts import *

folder = OUTPUT_DIR + "/granulometry/"
file_names = glob.glob(folder + "**/*.txt")

inpainted_r = [0 for _ in range(16)]
original_r = [0 for _ in range(16)]


def scaler(x):
    return log(500 * x + 1)
    # return x


def process(name):
    global inpainted_r, original_r

    # target = original_r
    # if '_inpainted' in name:
    #     target = inpainted_r

    with open(name) as f:
        l = [float(a) for a in next(f).split(' ') if a != '' and float(a) != 0]
        # target[min(len(l), 15)] += 1
        if (len(l) >= 4 and 'inpainted' not in name) or (len(l) < 4 and 'inpainted' in name):
            n = name.split('/')
            n[3] = 'is_mirror'
            ext = n[-1].split('.')
            ext[-2] += '_99'
            ext[-1] = 'png'
            n[-1] = '.'.join(ext)
            print('/'.join(n))
            im = cv2.imread('/'.join(n))
            cv2.imshow("dd", im)
            cv2.waitKey(0)
            # print(next(f).split(' '))

    color = 'b'
    if '_inpainted' in name:
        color = 'r'
    with open(name) as f:
        # print(next(f).split(' '))
        l = [scaler(float(a)) for a in next(f).split(' ') if a != '']
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

print(original_r)
print(inpainted_r)
plt.show()
