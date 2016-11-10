#include<iostream>
#include "base.h"
#include <opencv2/nonfree/nonfree.hpp>
using namespace std;
base* b;
void db2fv(); 
Mat extract_denseSift(const Mat& img); 
int main() {
   initModule_nonfree();
   b=new base();
   b->crop();
   db2fv();
}

void db2fv() {
    vector<vector<Mat>> sift;
    for (vector<Mat> crop: b->crops) {
        //_debugSize(crop);
        //TODO: parallel
        vector<Mat> tmp;
        for (Mat img:crop) 
            tmp.push_back(extract_denseSift(img));
        //_debugSize(tmp);
        sift.push_back(tmp);
    }
    //_debugSize(sift);
}
Mat extract_denseSift(const Mat& img) {
    Mat img_gray;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    DenseFeatureDetector detector;
	//SiftFeatureDetector detector;
    vector<KeyPoint> keypoints;
    detector.detect(img_gray, keypoints);
    Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create("SIFT");
    Mat descriptors;
    descriptorExtractor->compute( img_gray, keypoints, descriptors );
    // SIFT returns large negative values when it goes off the edge of the image.
    descriptors.setTo(0, descriptors<0);
    //_debugSize(descriptors);
    //_debug(descriptors.row(1));
    Mat rowSum;
    reduce(descriptors, rowSum, 1, CV_REDUCE_SUM);
    //_debug(rowSum);
    rowSum += 0.00001;
    repeat(rowSum,1,descriptors.cols,rowSum);
    //_debug(descriptors.row(0));
    descriptors /= rowSum;
    //_debug(descriptors.row(0));
    sqrt(descriptors,descriptors);
    //_debug(descriptors.row(0));
    Mat tmp;
    multiply(descriptors,descriptors,tmp);
    //_debug(tmp.row(0));
    reduce(tmp,rowSum,1,CV_REDUCE_SUM);
    //_debug(rowSum);
    sqrt(rowSum,rowSum);
    repeat(rowSum,1,descriptors.cols,rowSum);
    rowSum += 0.00001;
    descriptors /= rowSum;
    //_debug(descriptors.row(0));
    return descriptors;
    
}
