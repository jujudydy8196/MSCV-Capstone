import time
import sys, os
sys.path.append('/home/judy/capstone')
from cointerest import base, feature, landmark
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
plt.switch_backend('agg')
import numpy as np
print "video: %s" %(sys.argv[1])
video = sys.argv[1]

frameFolder = "./DATA/frame/"+ video +'/'
print "frameFolder: %s" %frameFolder 
cropFolder = "./DATA/crop/" + video +'/'
print "cropFolder: %s" %cropFolder

start = time.time()
# crop the image
print "croping..."
base.crop(frameFolder, cropFolder)
end = time.time()
print "crop takes %f s" %(end-start)

start = time.time()
# extract feature
fvFolder = ["./DATA/feature/FV/" + video +'/']
print "FV foder: %s" %fvFolder
feature.db2fv(cropFolder, 64, 12, fvFolder[0])
end = time.time()
print "dense sift takes %f s" %(end-start)

start = time.time()
coFolder= "./DATA/cosalient/" + video + '/'
cofvFolder = ["./DATA/feature/co/" + video + '/' ]
print "cofv foder: %s" %cofvFolder
feature.db2cosal(coFolder,cofvFolder[0])
end = time.time()
print "cosal takes %f s" %(end-start)

# Load data
start = time.time()
feat = landmark.load_scene_feature(fvFolder)
end = time.time()
print "load fv takes %f s" %(end-start)

start = time.time()
imp = landmark.load_scene_feature(cofvFolder)
end = time.time()
print "load cosal takes %f s" %(end-start)

# Find co-interest scenes
start = time.time()
idx, feat_all = landmark.find_cointerest(feat, imp,max_scenes=5, th=0.1)
# %time idx, feat_all = landmark.find_cointerest_noImp(feat, max_scenes=5)
end = time.time()
print "find cointerest takes %f s" %(end-start)

resultFolder= "./DATA/result/" +video +'/'
print resultFolder
if not os.path.exists(resultFolder):
    os.makedirs(resultFolder)
plt.figure()
plt.plot(np.concatenate(idx))
plt.axis('off')
# plt.show()
plt.savefig(resultFolder+"idx")

start = time.time()
base.visualize(frameFolder,idx,resultFolder)
end = time.time()
print " takes visualize %f s" %(end-start)


