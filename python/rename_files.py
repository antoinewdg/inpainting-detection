import glob
import os

from consts import *

file_names = glob.glob(INPUT_DIR + "/misc/*.png")
for name in file_names:
    if '_original' in name:
        # print(name.replace('_original', ''))
        os.rename(name, name.replace('_original', ''))
