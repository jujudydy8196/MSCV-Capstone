import sys, os
sys.path.append('/home/judy')
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

# crop the image
print "croping..."
base.crop(frameFolder, cropFolder)

# extract feature
fvFolder = ["./DATA/feature/FV/" + video +'/']
print "FV foder: %s" %fvFolder
feature.db2fv(cropFolder, 64, 12, fvFolder[0])

coFolder= "./DATA/cosalient/" + video + '/'
cofvFolder = ["./DATA/feature/co/" + video + '/' ]
print "cofv foder: %s" %cofvFolder
feature.db2cosal(coFolder,cofvFolder[0])

# Load data
feat = landmark.load_scene_feature(fvFolder)
imp = landmark.load_scene_feature(cofvFolder)
# Find co-interest scenes
idx, feat_all = landmark.find_cointerest(feat, imp,max_scenes=5, th=0.1)
# %time idx, feat_all = landmark.find_cointerest_noImp(feat, max_scenes=5)

resultFolder= "./DATA/result/" +video +'/'
print resultFolder
if not os.path.exists(resultFolder):
    os.makedirs(resultFolder)
plt.figure()
plt.plot(np.concatenate(idx))
plt.axis('off')
# plt.show()
plt.savefig(resultFolder+"idx")

base.visualize(frameFolder,idx,resultFolder)

