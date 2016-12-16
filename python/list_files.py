import glob

from consts import *

# file_names = glob.glob(INPUT_DIR + "/**/*.png") + \
#              glob.glob(INPUT_DIR + "/**/*.jpg") + \
#              glob.glob(INPUT_DIR + "/**/*.jpeg")

file_names = [INPUT_DIR + '/misc/bus_sky.png']

file_names = [name[len(INPUT_DIR) + 1:] for name in file_names if '_big' not in name]

with open(INPUT_DIR + "/index.txt", 'w') as f:
    f.write('\n'.join(file_names))

print('\n'.join(file_names))
