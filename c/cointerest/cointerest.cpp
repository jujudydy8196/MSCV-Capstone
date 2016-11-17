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
PCA learn_gmm(const Mat& feat, VlGMM* &); 
void convVec2Mat(const vector<vector<Mat>>& sift, Mat& feat); 
Mat generate_FV (const vector<Mat> &feat, const VlGMM* gmm, const PCA &pca) ;


struct gmmModel
{
  Mat  means;
  Mat  covs;
  Mat  priors;
  int Nclusters;
  int dimension;
};
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
    _debugSize(sift);
    VlGMM* gmm;
    PCA pca=learn_gmm(feat, gmm);
    vector<Mat> Fvs;
    for (int i=0; i<sift.size(); i++) {
       Fvs.push_back(generate_FV(sift[i], gmm, pca)); 
    }
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

PCA learn_gmm(const Mat& feat, VlGMM* &gmm) {
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
    PCA pca(feat_, Mat(), CV_PCA_DATA_AS_ROW, n_pca);
    feat_ = pca.project(feat_);
    //pca.eigenvectors.copyTo(feat_);
    //transpose(feat_,feat_);
    _debugSize(feat_);

    Mat *featPtr = &feat_;
    vl_size dimension = feat_.cols;  
    gmm = vl_gmm_new (VL_TYPE_FLOAT, dimension, n_gmm) ;
    vl_gmm_set_max_num_iterations (gmm, 30) ;
    vl_gmm_set_initialization (gmm,VlGMMKMeans);
    //cout << feat_.total() << endl;
    float* data = (float*)feat_.data;
    vl_gmm_cluster (gmm, data, feat_.rows);

    
    //cout << vl_gmm_get_means(gmm) << endl;
    //cout << Eigen::Map<Eigen::MatrixXd>( (double*) vl_gmm_get_priors(gmm), 1, n_gmm) << endl;
    //_debug(pca.eigenvectors);
    return pca;
}
Mat generate_FV (const vector<Mat> &feats, const VlGMM* gmm, const PCA &pca) {
    Mat Fv;
    //_debugSize(pca.project(feat[0]));
    vl_size dim = vl_gmm_get_dimension(gmm);
    vl_size nClusters = vl_gmm_get_num_clusters(gmm);
    vl_size numData = feats[0].rows;
    //cout << "dim " << dim << " nclusters: " << nClusters << endl;
    for (Mat feat : feats) {
        //_debugSize(pca.project(feat));
        float* enc = (float*)vl_malloc(sizeof(float) * 2 * dim * nClusters);
		vl_fisher_encode( enc, VL_TYPE_FLOAT,
                vl_gmm_get_means(gmm), dim, nClusters,
                vl_gmm_get_covariances(gmm),
                vl_gmm_get_priors(gmm),
                (float*)(pca.project(feat).data), numData,
                VL_FISHER_FLAG_IMPROVED) ;
        Mat fv(1, 2*dim*nClusters , CV_32F, enc);
        Fv.push_back(fv);
        vl_free(enc);
    }
    _debugSize(Fv);
    return Fv;
}
