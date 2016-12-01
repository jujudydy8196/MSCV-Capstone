//
//  cointerest.m
//  capstone
//
//  Created by Judy Chang on 11/29/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "cointerest.h"

base* b;


void cointerest() {

    initModule_nonfree();
    b=new base();
    b->crop();

    vector<Mat> FVs = db2fv();
    cout << FVs.size() << endl;
    _debugSize(FVs[0]);

//    Mat cosal=db2cosal();
//
//    Mat idx_all;
//    int clusters = findCointerest(FVs, cosal,idx_all);
////    cout << clusters << " clusters " << endl;
//    visualize(idx_all,clusters);
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
                Mat crop = im_gray(cv::Rect(i*b->gridSize,j*b->gridSize,b->gridSize,b->gridSize));
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
int findCointerest(const vector<Mat> &FVs, const Mat &cosal, Mat &idx_all) {
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
        Mat x = feat_all(cv::Rect(0,idx,FVs[i].cols ,FVs[i].rows));
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
    //_debugSize(M);
    float th=0.1;
    int max_scene=5;
    int scene_idx=0;
    idx_all = Mat::zeros(feat_all.rows, 1, CV_8U);
    Mat idx_occ = Mat::zeros(feat_all.rows, 1, CV_8U);
    //_debugSize(idx_all);
    for (int i=0; i<max_scene; i++) {
        Mat eig, evec;
        // TODO: eigen faster, get only 2
        eigen(M, eig, evec);
        //_debugSize(evec);
        evec(cv::Rect(0,0,evec.cols,2)).copyTo(evec);
        //_debugSize(evec);
        evec = feat_all * evec.t();
        //_debugSize(evec);
        
        Mat labels, centers;
        kmeans(evec, 2, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 300, 0.0001), 5, KMEANS_PP_CENTERS, centers);
        
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
        Mat mask;
        repeat(idx,1,feat_all.cols,mask);
        feat_all.setTo(0,mask);
        
        M -= (f.t() * f);
        Mat tmp_cosal = cosal.clone();
        tmp_cosal.setTo(0,1-idx);
        reduce(tmp_cosal, tmp_cosal, 0, CV_REDUCE_SUM);
        float prob = tmp_cosal.at<float>(0,0) / countNonZero(idx);
        float score = prob * eig.at<float>(1,0);
        cout << "prob : " << prob << " score: " << score << endl;
        
        if (score < th)
            break;
        for (int i=0; i<idx.rows; i++) {
            if (idx.at<uchar>(i,0)==1 && idx_occ.at<uchar>(i,0)==0){
                idx_all.at<uchar>(i,0) = scene_idx+1;
                idx_occ.at<uchar>(i,0)=1;
            }
        }
        scene_idx++;
    }
    
    idx_all = idx_all.reshape(0,b->img_num);
    return scene_idx;
    
    
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
    _debugSize(feat_);
    
    Mat *featPtr = &feat_;
    vl_size dimension = feat_.cols;
    gmm = vl_gmm_new (VL_TYPE_FLOAT, dimension, n_gmm) ;
    vl_gmm_set_max_num_iterations (gmm, 30) ;
    vl_gmm_set_initialization (gmm,VlGMMKMeans);
    float* data = (float*)feat_.data;
    vl_gmm_cluster (gmm, data, feat_.rows);
    
    
    return pca;
}
Mat generate_FV (const vector<Mat> &feats, const VlGMM* gmm, const PCA &pca) {
    Mat Fv;
    //_debugSize(pca.project(feat[0]));
    vl_size dim = vl_gmm_get_dimension(gmm);
    vl_size nClusters = vl_gmm_get_num_clusters(gmm);
    vl_size numData = feats[0].rows;
    for (Mat feat : feats) {
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

void visualize (const Mat& feat_all, const int clusters) {
    Mat count = Mat::zeros(feat_all.rows,clusters, CV_8U);
    _debugSize(count);
    for (int i=1; i<=clusters; i++) {
        int maxIdx=-1, maxCount=-1;
        cout << "cluster: " << i << endl;
        for (int frame=0; frame<b->img_num; frame++) {
            int c=countNonZero(feat_all.row(frame)==i);
            cout << "frame " << frame << " " << c << endl;
            if (c >= maxCount) {
                maxCount = c;
                maxIdx = frame;
            }
        }
        cout << "maxIdx " << maxIdx << endl;
        string str = b->result_path + "result_" + to_string(i) + ".png";
        imwrite(str, b->data_image_cv[maxIdx]);
    }
    
}
