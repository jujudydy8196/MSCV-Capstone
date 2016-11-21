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
vector<Mat> db2fv(); 
Mat extract_denseSift(const Mat& img); 
PCA learn_gmm(const Mat& feat, VlGMM* &); 
void convVec2Mat(const vector<vector<Mat>>& sift, Mat& feat); 
Mat generate_FV (const vector<Mat> &feat, const VlGMM* gmm, const PCA &pca) ;
Mat db2cosal() ;
void findCointerest(const vector<Mat> &FVs, const Mat &cosal);

int main() {
   initModule_nonfree();
   b=new base();
   b->crop();
   vector<Mat> FVs = db2fv();
   //cout << "FVs len: " << FVs.size() << endl;
   //_debugSize(FVs[0]);
   Mat cosal=db2cosal();
   findCointerest(FVs, cosal);
}

vector<Mat> db2fv() {
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
    return Fvs;
}
Mat db2cosal() {
//vector<vector<float>> db2cosal() {
    float grayMax=0.0;
    Mat cropss;
    //vector<vector<float>> cropss;
    for (int i=0; i<b->img_num; i++) {
        string fileName= b->cosal_path+b->files_list[i].substr(0,3)+"_cosaliency.png";
        Mat im=imread(fileName.c_str(),1);
        Mat im_gray;
        cvtColor(im, im_gray, CV_BGR2GRAY);
        //cout << b->imsize << endl;
        int w = b->orgSize.width / b->gridSize;
        int h = b->orgSize.height / b->gridSize;
        //vector<float> crops;
        for (int i=0; i<w; i++) {
            for (int j=0; j<h; j++ ){
                Mat crop = im_gray(Rect(i*b->gridSize,j*b->gridSize,b->gridSize,b->gridSize));
                //cout <<"mean: " << mean(crop) << endl;
                cropss.push_back((float)mean(crop).val[0]);
                //crops.push_back((float)mean(crop).val[0]);
                grayMax = max(grayMax, (float)mean(crop).val[0]);
            }
        }
        //cropss.push_back(crops);
    }
    cropss /= grayMax;
    return cropss;
}
void findCointerest(const vector<Mat> &FVs, const Mat &cosal) {
    Mat feat_all;
    for (auto &fv:FVs)
        feat_all.push_back(fv);
    Scalar mean, std;
    meanStdDev(feat_all,mean,std);
    feat_all -= mean[0];
    feat_all /= (std[0]+0.0001);
    int idx=0;
    Mat degree; //, frame_id;
    for (int i=0; i<b->img_num; i++) {
        //frame_id.push_back(Mat(FVs[i].rows,1,CV_8UC1,Scalar(i)));
        Mat x = feat_all(Rect(0,idx,FVs[i].cols ,FVs[i].rows));
        idx += FVs[i].rows;
        //_debugSize(x);
        Mat tmp = x*x.t();
        Mat mask = tmp < 0;
        mask.convertTo(mask, CV_8U, 1.0/255.0);
        tmp.setTo(0, mask);
        //_debugSize(tmp);
        reduce(tmp,tmp,1,CV_REDUCE_SUM);
        //_debugSize(tmp);
        degree.push_back(tmp);
        //break;
    }
    //_debugSize(frame_id);
    sqrt(degree,degree);
    divide(feat_all, repeat(degree, 1, feat_all.cols), feat_all);
    //feat_all /= (repeat(degree, 1, feat_all.cols));
     
    multiply(feat_all, repeat(cosal, 1, feat_all.cols), feat_all);
    Mat M = feat_all.t() * feat_all;
    _debugSize(M);
    float th=0.1;
    int max_scene=5;
    int scene_idx=0;
    Mat idx_all = Mat::zeros(feat_all.rows, 1, CV_8U);
    Mat idx_occ = Mat::zeros(feat_all.rows, 1, CV_8U);
    _debugSize(idx_all);
    for (int i=0; i<max_scene; i++) {
        Mat eig, evec;
        // TODO: eigen faster, get only 2
        eigen(M, eig, evec);
        //_debug(eig);
        _debugSize(evec);
        evec(Rect(0,0,evec.cols,2)).copyTo(evec);
        _debugSize(evec);
        evec = feat_all * evec.t();
        _debugSize(evec);

        Mat labels, centers;
        kmeans(evec, 2, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 300, 0.0001), 5, KMEANS_PP_CENTERS, centers);
        _debugSize(centers);
        _debugSize(labels);

        vector<double> norms;
        for (int i=0; i<2; i++) 
            norms.push_back(norm(centers.row(i)));
        int tmp = (norms[0] > norms[1]) ? 0 : 1;
        Mat idx = labels == tmp;
        idx.convertTo(idx, CV_8U, 1.0/255.0);
        //_debug(idx);
        Mat f;//, v;
        for (int i=0; i<idx.rows; i++) {
            if (idx.at<uchar>(i,0)==1){ 
                f.push_back(feat_all.row(i));
                //v.push_back(frame_id.row(i));
            }
        }
        //_debugSize(v);
        Mat mask;
        repeat(idx,1,feat_all.cols,mask);
        //feat_all.copyTo(f,idx);
        cout << countNonZero(idx) << endl;
        _debugSize(f);
        feat_all.setTo(0,mask);
        //_debugSize(f.t()*f);

        M -= (f.t() * f);
        Mat tmp_cosal = cosal.clone();
        //_debugSize(idx);
        //_debugSize(tmp_cosal);
        tmp_cosal.setTo(0,1-idx);
        //cout << countNonZero(idx) << " " << countNonZero(tmp_cosal) << endl;
        _debugSize(tmp_cosal);
        reduce(tmp_cosal, tmp_cosal, 0, CV_REDUCE_SUM);
        _debugSize(tmp_cosal);
        _debug(tmp_cosal);
        float prob = tmp_cosal.at<float>(0,0) / countNonZero(idx);
        _debugSize(eig);
        float score = prob * eig.at<float>(1,0);
        cout << "prob : " << prob << " score: " << score << endl;
        
        //vector<float> uni = unique(v, true);
        //cout << uni.size() << endl;
        if (score < th)
            break;
        for (int i=0; i<idx.rows; i++) {
            if (idx.at<uchar>(i,0)==1 && idx_occ.at<uchar>(i,0)==0){ 
                idx_all.at<uchar>(i,0) = scene_idx+1;
                idx_occ.at<uchar>(i,0)=1;
            }
        }
        scene_idx++;
        cout << countNonZero(idx_all) << endl;
    }
    
    //_debug(idx_all);
    idx_all = idx_all.reshape(0,b->img_num);
    _debug(idx_all);
    _debugSize(idx_all);



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
    //for (int i=0; i<2; i++) {
        //Mat feat = feats[i];
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
    //_debug(Fv);
    Mat sign;
    divide(Fv, abs(Fv), sign);
    //_debug(sign);
    sqrt(abs(Fv),Fv);
    multiply(Fv,sign,Fv);
    Mat tmp;
    pow(Fv,2,tmp);
    reduce(tmp, tmp, 1, CV_REDUCE_SUM);
    sqrt(tmp,tmp);
    tmp += 0.00001;
    repeat(tmp,1,Fv.cols,tmp);
    Fv /= tmp;
    //_debugSize(Fv);
    return Fv;
}
