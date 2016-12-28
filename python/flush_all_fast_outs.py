import glob

from consts import *

file_names = glob.glob(OUTPUT_DIR + "/**/*.*")
print(file_names)
