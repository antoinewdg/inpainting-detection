import glob

from consts import *

file_names = glob.glob(INPUT_DIR + "/**/*.png") + \
             glob.glob(INPUT_DIR + "/**/*.jpg") + \
             glob.glob(INPUT_DIR + "/**/*.jpeg")


def key_f(x):
    if "facade" in x:
        return 99999, x
    else:
        return 0, x


file_names.sort(key=key_f)

# file_names = [INPUT_DIR + '/misc/bus_sky.png']

file_names = [name[len(INPUT_DIR) + 1:] for name in file_names if '_big' not in name]
# file_names = [n for n in file_names if 'facade' not in n and 'special_case' not in n]

# file_names = [name for name in file_names if 'grip' in name and 'rescaled' in name]
file_names = [name for name in file_names if 'grip' in name and
              ('noise_0.04' in name or 'jpeg_80' in name)]

with open(INPUT_DIR + "/index.txt", 'w') as f:
    f.write('\n'.join(file_names))

print('\n'.join(file_names))
