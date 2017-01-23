import cv2
import numpy as np
import glob
from matplotlib import pyplot as plt
import seaborn as sbn
import os

INPUT_DIR = "../files/in"
OUTPUT_DIR = "../files/out"


def clean_name(name):
    return '/'.join(name.split('/')[-2:]).rpartition('.')[0]


def get_full_name(name, dir, ext):
    return OUTPUT_DIR + "/%s/%s.%s" % (dir, name, ext)
