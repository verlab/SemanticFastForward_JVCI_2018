#!/usr/bin/python
# coding: utf-8

import cv2, sys
import sys
import mycaffe
import os.path
import caffe
from tqdm import tqdm

def convert_cvmat_caffeio(img):
    img = img / 255.
    img = img[:,:,(2,1,0)]
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

cap = cv2.VideoCapture(sys.argv[1])
framecount = cap.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT)
ext = sys.argv[1].split('.')[-1]

myc = mycaffe.mycaffe()
myc.CAFFEMODEL_NAME = sys.argv[2]
myc.DEPLOY_NAME = sys.argv[3]
myc.MEAN_NPY_FILE = sys.argv[4]

FRAMES_PER_VID = 90

out_file = open(sys.argv[1].replace('.'+ext, '_coolnet_extracted.csv'),'w')

if '-gpu' in sys.argv:
    myc.get_net(mode = 'gpu')
else:
    myc.get_net(mode = 'cpu')

for x in tqdm(xrange(int(framecount))):
    
    vid_id = x / FRAMES_PER_VID

    # Load Image
    image = convert_cvmat_caffeio(cap.read()[1])
    image = caffe.io.resize_image(image, (227 ,227))
    
    # Predict
    l = myc.get_features(crop_size=227, test_image=image, channels = 3)
    out_file.write('vid'+str(vid_id)+'_frame_'+str(x))
    for y in l:
        out_file.write(','+str(y))
    
    out_file.write('\n')

out_file.close()