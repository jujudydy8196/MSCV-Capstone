//
//  cointerest.h
//  capstone
//
//  Created by Judy Chang on 11/29/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#ifndef cointerest_h
#define cointerest_h

#include <iostream>
#include "config.h"

#include <opencv2/nonfree/nonfree.hpp>
//#include <eigen3/Eigen/Dense>
//#include "cointerest/base.h"
//extern "C" {
//#include "src/vlfeat/vl/generic.h"
//#include "src/vlfeat/vl/gmm.h"
//#include "src/vlfeat/vl/fisher.h"
//}
#include "generic.h"
#include "gmm.h"
#include "fisher.h"

using namespace cv;
using namespace std;

void cointerest();
void convVec2Mat(const vector<vector<Mat>>& sift, Mat& feat);

vector<Mat> db2fv();
Mat extract_denseSift(const Mat& img);
PCA learn_gmm(const Mat& feat, VlGMM* &);
Mat generate_FV (const vector<Mat> &feat, const VlGMM* gmm, const PCA &pca) ;
Mat db2cosal() ;
int findCointerest(const vector<Mat> &FVs, const Mat &cosal, Mat& idx_all);
void visualize (const Mat& feat_all, const int clusters);

#endif /* cointerest_h */
