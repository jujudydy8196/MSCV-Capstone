#include<iostream>
#include "base.h"
#include <opencv2/nonfree/nonfree.hpp>
#include <eigen3/Eigen/Dense>
extern "C" {
  #include <../src/vlfeat/vl/generic.h>
  //#include <../src/vlfeat/vl/dsift.h>
  #include <../src/vlfeat/vl/gmm.h>
  #include <../src/vlfeat/vl/fisher.h>
  //#include <../src/vlfeat/vl/svm.h>
}
using namespace std;
base* b;
void db2fv(); 
Mat extract_denseSift(const Mat& img); 
void learn_gmm(const Mat& feat); 
void convVec2Mat(const vector<vector<Mat>>& sift, Mat& feat); 
int main() {
   initModule_nonfree();
   b=new base();
   b->crop();
   db2fv();
}

void db2fv() {
    vector<vector<Mat>> sift;
    for (vector<Mat> crop: b->crops) {
        //TODO: parallel
        vector<Mat> tmp;
        for (Mat img:crop) 
            tmp.push_back(extract_denseSift(img));
        sift.push_back(tmp);
            //feat.push_back(extract_denseSift(img));
    }
    
    Mat feat;
    convVec2Mat(sift,feat);
    _debugSize(feat);
    learn_gmm(feat);
}
void convVec2Mat(const vector<vector<Mat>>& sift, Mat& feat) {
    for (vector<Mat> vec:sift) {
        for (Mat f:vec)
            feat.push_back(f);
    }

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

void learn_gmm(const Mat& feat) {
    int n_pca = b->n_pca;
    int n_gmm = b->n_gmm;
    Mat feat_;
    if (feat.rows > n_gmm*1000) {
        vector<int> seeds;
        for (int cont = 0; cont < feat.rows; cont++)
            seeds.push_back(cont);
        randShuffle(seeds);
        for (int i=0; i<n_gmm*1000; i++)
            feat_.push_back(feat.row(seeds[i]));
    }
    else
        feat.copyTo(feat_);
    _debugSize(feat_);
    PCA pca(feat_, Mat(), CV_PCA_DATA_AS_COL, n_pca);
    pca.eigenvectors.copyTo(feat_);
    transpose(feat_,feat_);
    _debugSize(feat_);

    Mat *featPtr = &feat_;
    vl_size dimension = feat_.cols;  
    VlGMM* gmm = vl_gmm_new (VL_TYPE_DOUBLE, dimension, n_gmm) ;
    vl_gmm_set_max_num_iterations (gmm, 30) ;
    vl_gmm_set_initialization (gmm,VlGMMKMeans);
    cout << "dim: " << dimension << endl;
    cout << feat_.total() << endl;
    float* data = (float*)feat_.data;
    vl_gmm_cluster (gmm, data, feat_.rows);
    //cout << vl_gmm_get_means(gmm) << endl;
    cout << Eigen::Map<Eigen::MatrixXd>( (double*) vl_gmm_get_means(gmm), 1, dimension*n_gmm) << endl;
    //_debug(pca.eigenvectors);
}
