import glob

from consts import *

names = glob.glob(INPUT_DIR + '/grip/TP_*.png')
for n in names:
    print(n)
    # new_n = n.split('/')
    # new_n = '/'.join(new_n[:-2] + new_n[-1:])
    # new_n = new_n.replace('_copy', '_inpainted')
    # # print(new_n)
    os.rename(n, n.replace('TP_', 'IMG_'))
