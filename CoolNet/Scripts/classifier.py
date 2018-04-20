#!/usr/bin/python
# coding: utf-8

import caffe

import cv2
import sys
import sys
import mycaffe
import os.path
from tqdm import tqdm


def is_cv2():
    # if we are using OpenCV 2, then our cv2.__version__ will start
    # with '2.'
    return check_opencv_version("2.")


def check_opencv_version(major, lib=None):
    # if the supplied library is None, import OpenCV
    if lib is None:
        import cv2 as lib

    # return whether or not the current OpenCV version matches the
    # major version number
    return lib.__version__.startswith(major)


def convert_cvmat_caffeio(img):
    img = img / 255.
    img = img[:, :, (2, 1, 0)]
    return img


argc = len(sys.argv)

if argc < 5:
    print 'Missing Arguments'
    print'''
        USAGE: python classifier.py 
                    <path/to/video.mp4>
                    <path/to/pretrained/mode.caffemodel>
                    <path/to/coolnet_deploy.prototxt>
                    <path/to/mean/file.npy>
                    <flags [-gpu or -cpu]>'
            
                    -gpu for GPU classification --> default
                    -cpu for CPU classification 
        '''
    exit()

if is_cv2():
    PROP_FRAME_COUNT = cv2.cv.CV_CAP_PROP_FRAME_COUNT
else:
    PROP_FRAME_COUNT = cv2.CAP_PROP_FRAME_COUNT

cap = cv2.VideoCapture(sys.argv[1])
framecount = cap.get(PROP_FRAME_COUNT)
ext = sys.argv[1].split('.')[-1]

myc = mycaffe.mycaffe()
myc.CAFFEMODEL_NAME = sys.argv[2]
myc.DEPLOY_NAME = sys.argv[3]
myc.MEAN_NPY_FILE = sys.argv[4]

out_file = open(sys.argv[1].replace('.'+ext, '_class.txt'), 'w')

if '-gpu' in sys.argv:
    myc.get_net(mode='gpu')
else:
    myc.get_net(mode='cpu')


for x in tqdm(xrange(framecount)):

    # Load Image
    image = convert_cvmat_caffeio(cap.read()[1])
    image = caffe.io.resize_image(image, (224, 224))

    # Predict
    r = myc.get_who(crop_size=224, test_image=image, channels=3)
    out_file.write(str(x)+','+str(r[1])+'\n')
