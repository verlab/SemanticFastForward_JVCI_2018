import os, sys, random, glob, numpy, caffe
import matplotlib.pyplot as plt
class mycaffe:
	'Class to train, load and use a caffe CNN'
	def __init__(self,
	MEAN_BINARY_FILE = 'mean.binary_proto',
	CAFFEMODEL_NAME = 'some_net_train_iter_10000.caffemodel',
	DEPLOY_NAME = 'deploy.prototxt',
	MEAN_NPY_FILE = '',
	NET = '',
	TRANSFORMER = ''):
		self.CAFFEMODEL_NAME = CAFFEMODEL_NAME
		self.DEPLOY_NAME = DEPLOY_NAME
		self.MEAN_NPY_FILE = MEAN_NPY_FILE
		self.TRANSFORMER = TRANSFORMER
		self.NET = NET

	def run_net(self, test_image, crop_size, channels):
		net = self.NET
		transformer = self.TRANSFORMER
		net.blobs['data'].reshape(1,channels,crop_size,crop_size)
		net.blobs['data'].data[...] = transformer.preprocess('data', test_image)
		out = net.forward()
		return out

	def get_who(self, test_image, crop_size = 224, channels = 3):
		net = self.NET
		out = self.run_net(test_image, crop_size, channels)
		prob = net.blobs['prob'].data[0]#.flatten().argsort()[-1:-6:-1]
		return prob

	def get_features(self, test_image, feature_layer='fc7', crop_size = 224, channels = 3):
		net = self.NET
		out = self.run_net(test_image, crop_size, channels)
		features = net.blobs[feature_layer].data.copy()
		features = numpy.reshape(features, (features.shape[0], -1))
		return features.tolist()[0]

	def get_net (self, mode = 'gpu'):
		if mode == 'gpu':
			caffe.set_mode_gpu()
		else:
			caffe.set_mode_cpu()
		net = caffe.Net(self.DEPLOY_NAME, self.CAFFEMODEL_NAME, caffe.TEST)
		transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
		if self.MEAN_NPY_FILE == '':
			transformer.set_mean('data', numpy.array([104,117,124]))
		else:
			transformer.set_mean('data', numpy.load(self.MEAN_NPY_FILE).mean(1).mean(1))
		transformer.set_transpose('data', (2,0,1))
#		transformer.set_channel_swap('data', (2,1,0))
		transformer.set_raw_scale('data', 255.0)
		self.NET = net
		self.TRANSFORMER = transformer
		return net, transformer