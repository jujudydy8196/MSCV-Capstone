//
//  cosaliency.h
//  capstone
//
//  Created by Judy Chang on 11/28/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#ifndef cosaliency_h
#define cosaliency_h
#include "config.h"

using namespace cv;

vector<Mat> cosaliency();
Mat cosaliency_co() ;
Mat cosaliency_recording(cosal_config* set) ;

vector<Mat> saveResult(const Mat &result);
void GetImVector(const Mat &img, Mat &featureVec, Mat &disVec);

Mat GetSalWeight(const Mat &centers, const Mat &labels);
Mat GetCoWeight( const Mat &labels );
void Gauss_normal(Mat &input, float center, float sigma );
Mat GetPositionW(const Mat labels, const Mat disVec, int bin_num);
Mat Cluster2img(const Mat &Cluster_Map, const Mat &SaliencyWeight_all, const int bin_num);
Mat SingleSaliencyMain();
Mat CoSaliencyMain();

#endif /* cosaliency_h */
