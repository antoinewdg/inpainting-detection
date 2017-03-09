import glob

from consts import *

file_names = glob.glob(INPUT_DIR + '/**/*.png')
file_names = [n for n in file_names if 'noise' not in n and 'jpeg' not in n and 'rescaled' not in n]
file_names = [n for n in file_names if 'grip' not in n]
file_names = [n for n in file_names if 'report' in n ]


for i in range(1, 6):
    f = 0.02 * i
    dev = 0.5 * 255 * f
    # dirname = INPUT_DIR + '/grip_noise_%s' % f
    # try:
    #     os.makedirs(dirname)
    # except:
    #     pass
    print("Generating noise %s" % dev)
    for fname in file_names:
        # print(clean_name(fname))
        dir_name, _, name = clean_name(fname).partition('/')
        image = cv2.imread(fname).astype(float)
        noise = np.zeros(image.shape, dtype=float)
        cv2.randn(noise, (0, 0, 0), (dev, dev, dev))
        #
        image += noise
        image[image < 0] = 0
        image[image > 255] = 255
        image = image.astype(np.uint8)

        # out_dir_path = INPUT_DIR + '/%s_noise_%s' % (dir_name, f)
        out_dir_path = INPUT_DIR + '/report'
        # try:
        #     os.makedirs(out_dir_path)
        # except:
        #     pass
        out_name = '%s/%s_noise_%s.png' % (out_dir_path, name, f)
        # print("Writing %s" % out_name)
        cv2.imwrite(out_name, image)

# for i in range(1, 10):
#     noise = np.zeros((100, 100, 3), dtype=float)
#     dev = 2.5 * i
#     back = np.ones((100, 100, 3), dtype=float) * 125
#     cv2.randn(noise, (0, 0, 0), (dev, dev, dev))
#     cv2.imwrite('../test/assets/misc/noise_' + str(dev) + '.png', back + noise)
# print('..files/noise/noise_' + str(dev) + '.png')
# noisy = im + noise
# noisy[noisy < 0] = 0
# noisy[noisy > 255] = 255
# # cv2.imshow("d", noisy.astype(np.uint8))
# # cv2.waitKey(0)
# root, ext = name.split('.')
# new_name = root + '_noisy_' + str(dev) + '.' + ext
# cv2.imwrite(INPUT_DIR + '/' + new_name, noisy)
