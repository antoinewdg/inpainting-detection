import glob

from consts import *

for i in range(1, 10):
    noise = np.zeros((500, 500, 3), dtype=float)
    dev = 2.5 * i
    back = np.ones((500, 500, 3), dtype=float) * 125
    cv2.randn(noise, (0, 0, 0), (dev, dev, dev))
    cv2.imwrite(INPUT_DIR + '/../noise/noise_' + str(dev) + '.png', back + noise)
    # print('..files/noise/noise_' + str(dev) + '.png')
    # noisy = im + noise
    # noisy[noisy < 0] = 0
    # noisy[noisy > 255] = 255
    # # cv2.imshow("d", noisy.astype(np.uint8))
    # # cv2.waitKey(0)
    # root, ext = name.split('.')
    # new_name = root + '_noisy_' + str(dev) + '.' + ext
    # cv2.imwrite(INPUT_DIR + '/' + new_name, noisy)
