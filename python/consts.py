import cv2
import numpy as np
from matplotlib import pyplot as plt

INPUT_DIR = "../files/in"
OUTPUT_DIR = "../files/out"


def clean_name(name):
    return '/'.join(name.split('/')[-2:])
