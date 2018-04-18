#!/usr/bin/python
# coding: utf-8


import sys, caffe, numpy

if len(sys.argv) < 2:
    print '''
            USAGE:
            python convert_mean.py <path/to/mean.binaryproto>    

    '''
    exit();


mean_file = sys.argv[1]
blob = caffe.proto.caffe_pb2.BlobProto()
data = open( mean_file , 'rb' ).read()
blob.ParseFromString(data)
arr = numpy.array( caffe.io.blobproto_to_array(blob) )
out = arr[0]
npy_mean_file = mean_file.replace(".binaryproto", ".npy")
numpy.save( npy_mean_file , out )