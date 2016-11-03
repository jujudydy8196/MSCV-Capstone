#include <stdio.h>
#include <iostream>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/eigen.hpp>
#include "config.h"
#include <ctime>
#include <math.h>
//#include "SingleSaliency.h"

using namespace std;
using namespace cv;
using Eigen::MatrixXd;

#define _debugSize(x) cout << "_debugSize: " << #x << " : " << x.size() << endl;
#define _debug(x) cout << "_debug: " << #x << " : " << x << endl << endl;
config* settings;

void GetImVector(const Mat img, Mat &featureVec, Mat &disVec); 
Mat GetSalWeight(const Mat centers, const Mat labels); 
Mat GetPositionW(const Mat labels, const Mat disVec); 
Mat SingleSaliencyMain(const vector<Mat>);
void Gauss_normal(Mat &input, float center, float sigma ) ;
Mat Cluster2img(const Mat Cluster_Map, const Mat SaliencyWeight_all, const int bin_num) ;
Mat CoSaliencyMain(const vector<Mat> data); 
Mat GetCoWeight( const Mat labels ); 
int main() {
    settings = new config("IMG_5094");
    vector<Mat> data_image_cv;
    for (string fileName : settings->files_list) {
        Mat m=imread((settings->img_path+fileName).c_str());
        //m.convertTo(m, CV_32FC3);
        resize(m,m,Size(settings->scale, settings->scale), INTER_CUBIC);
        data_image_cv.push_back(m);
    }
    //Mat result_sig_map = SingleSaliencyMain(data_image_cv);
    //imwrite( "single.png", result_sig_map*255 );
    Mat result_cos_map = CoSaliencyMain(data_image_cv);

    vector<Mat> test;
    for (int i=0; i<2; i++) {
        Mat tmp = result_cos_map(Rect(i*settings->scale,0,settings->scale,settings->scale));
        test.push_back(tmp);
    }
    _debug(test[0]) ;
    _debug(test[1]) ;
    //Mat diff = test[0] != test[1];
    //_debug(diff);
    _debugSize(result_cos_map);
    //Mat result = result_sig_map.mul(result_cos_map);
    //_debug(result_cos_map);
    imwrite( "cos.png", result_cos_map*255 );
    //imwrite( "result.png", result*255 );
    //_debugSize(result);
}

void GetImVector(const Mat img, Mat &featureVec, Mat &disVec) {
    //imshow("img1",img);
    Mat img2;
    cvtColor(img, img2, CV_BGR2Lab);
    img2.convertTo(img2, CV_32F);
    //_debug(img2);
    featureVec = img2.reshape(1,settings->scale*settings->scale);
    //_debugSize(featureVec);
    for (int row=0; row<settings->scale; row++) {
        for (int col=0; col<settings->scale; col++ ) {
            disVec.at<float>(row*settings->scale+col)=round(sqrt(pow(row-settings->scale/2,2)+pow(col-settings->scale/2,2)));
        }
    }
}
Mat GetSalWeight(const Mat centers, const Mat labels) {
    int bin_num=centers.rows;
    vector<float> bin_weight(bin_num,0);
    for (int i=0; i<bin_num; i++) {
        bin_weight[i] = (float)cv::countNonZero(labels==i) / labels.rows;
    }
    Mat y = Mat::zeros(bin_num, bin_num, CV_32F);
    for (int i=0; i<bin_num; i++) {
        for (int j=i; j<bin_num; j++) {
            y.at<float>(i,j) = y.at<float>(j,i) = norm(centers.row(i)-centers.row(j));
            y.at<float>(i,j) *= bin_weight[i];
            y.at<float>(j,i) *= bin_weight[j];
        }
    }
    
    Mat Sal_weight;
    reduce(y,Sal_weight,0,CV_REDUCE_SUM, CV_32F);
    return Sal_weight.t();
}
Mat GetCoWeight( const Mat labels ) {
    int img_num=settings->img_num; //labels.rows/(settings->scale*settings->scale);
    int bin_num=settings->Bin_num; //maxVal;
    Mat img_idx=labels.reshape(0,settings->scale);
    //_debugSize(img_idx);
    Mat bin_idx= Mat::zeros(bin_num,img_num,CV_32F);
    //_debugSize(bin_idx);
    //_debug(labels);
    for (int i=0; i<bin_num; i++) {
        float tmp=cv::countNonZero(img_idx==i);
        //cout << "count label==" << i << " : " << tmp << endl;
        for (int j=0; j<img_num; j++) {
            //_debugSize(img_idx(Rect(j*settings->scale, 0, settings->scale, settings->scale)));
        //Saliency_sig_final.copyTo(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
            bin_idx.at<float>(i,j)=(float) (cv::countNonZero(img_idx(Rect(j*settings->scale,0,settings->scale, settings->scale))==i))/tmp;
            //cout << (float)cv::countNonZero(img_idx(Rect(j*settings->scale,0,settings->scale, settings->scale))==i) << endl;
        }
    }
    //_debug(bin_idx);
    Mat co_weight=Mat::zeros(bin_num,1,CV_32F);
    for (int i=0; i<bin_num; i++) {
       //cout << sum(bin_idx.row(i)) << endl;
       bin_idx.row(i) /= sum(bin_idx.row(i))[0];
       Scalar mean, std;
       meanStdDev(bin_idx.row(i),mean,std);
       //cout << mean <<" "<< std << endl;
       co_weight.at<float>(i)=mean[0] / (std[0]+1);
    }
    //float data[22]={ 0.0824, 0.0885, 0.0886, 0.0849, 0.0857, 0.0880, 0.0873, 0.0868, 0.086,  0.085,  0.086,  0.086,  0.086,  0.087,  0.084,  0.088,  0.087,  0.085,  0.086,  0.085,  0.0837, 0.0848};
    //Mat co_weight=Mat(bin_num,1, CV_32F, &data);
    //_debug(co_weight);
    return co_weight;
}
Mat GetPositionW(const Mat labels, const Mat disVec, int bin_num) {
    Mat disWeight = Mat::zeros(bin_num, 1, CV_32F);
    for (int i=0; i<bin_num; i++) {
        Mat mask=labels==i;
        mask.convertTo(mask,CV_8U,1.0/255.0);
        Mat disVecIdx;
        disVec.copyTo(disVecIdx,mask);
        disWeight.at<float>(i)=sum(disVecIdx)[0]/countNonZero(labels==i);

    }
    Gauss_normal(disWeight,0,settings->scale);
   return disWeight;

}
void Gauss_normal(Mat &input, float center, float sigma ) {
    for (int i=0; i<input.rows; i++) {
        float x=input.at<float>(i);
        input.at<float>(i)=exp(-(x-center)*(x-center)/(2*sigma*sigma));
    }
}
Mat Cluster2img(const Mat Cluster_Map, const Mat SaliencyWeight_all, const int bin_num) {
    Mat Saliency_sig_temp = Mat(Cluster_Map.size(), CV_32F); //Cluster_Map; //Mat::zeros(settings->scale, settings->scale, CV_32F);
    //_debug(Cluster_Map);
    //_debugSize(SaliencyWeight_all);

    for (int i=0; i<bin_num; i++) {
        Mat mask= Cluster_Map==i;
        //_debug(mask);
        mask.convertTo(mask, CV_8U, 1.0/255.0);
        //cout << SaliencyWeight_all.at<float>(i) << endl;
        Saliency_sig_temp.setTo(SaliencyWeight_all.at<float>(i),mask);
    }
    //_debugSize(Saliency_sig_temp);
    Mat Saliency_Map_single ;
    GaussianBlur(Saliency_sig_temp, Saliency_Map_single, Size(3,3), 3);
    _debugSize(Saliency_Map_single);
    return Saliency_Map_single;
}
Mat SingleSaliencyMain(const vector<Mat> data) {
    Mat Saliency_Map_single = Mat::zeros(settings->scale, settings->scale * settings->img_num, CV_32F);
    for (int i=0; i<settings->img_num; i++) { //Mat img:data
        Mat img=data[i];
        //_debug(img);
        Mat featureVec= Mat::zeros(settings->scale*settings->scale,3,CV_32F);
        Mat disVec = Mat::zeros(settings->scale*settings->scale,1,CV_32F);
        GetImVector(img, featureVec, disVec);
        //_debugSize(featureVec);
        //_debugSize(disVec);
        Mat labels, centers;
        kmeans(featureVec, settings->Bin_num_single, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), 5, KMEANS_PP_CENTERS, centers);
        //_debugSize(labels);
        //_debugSize(centers);
        Mat Cluster_Map=labels.reshape(0,settings->scale);
        //_debugSize(Cluster_Map);
        //_debug(Cluster_Map);
        Mat Sal_weight = GetSalWeight(centers,labels);
        Mat Dis_weight = GetPositionW(labels, disVec, settings->Bin_num_single);
        
        double minVal,maxVal;
        minMaxLoc( Sal_weight, &minVal, &maxVal);
        Gauss_normal(Sal_weight, maxVal, (maxVal-minVal)/2);
        minMaxLoc( Dis_weight, &minVal, &maxVal);
        Gauss_normal(Dis_weight, maxVal, (maxVal-minVal)/2);
        //_debug(Sal_weight);
        //_debug(Dis_weight);
        //_debug(Sal_weight.mul(Dis_weight));
        Mat SaliencyWeight_all=Sal_weight.mul(Dis_weight);
        Mat Saliency_sig_final=Cluster2img(Cluster_Map, SaliencyWeight_all, settings->Bin_num_single);
        //_debugSize(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale))); 
        //cout << countNonZero(Saliency_sig_final) << endl;
        //Mat mask = Saliency_Map_single(Range::all(), Range(i*settings->scale, (i+1)*settings->scale-1));
        Saliency_sig_final.copyTo(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
        //cout << countNonZero(Saliency_Map_single) << endl;
        //_debug(Saliency_Map_single);
        //break;
    }
    return Saliency_Map_single;

}

Mat CoSaliencyMain(const vector<Mat> data) {
    Mat All_vector, All_disVector;
    for (Mat img:data) {
        Mat featureVec= Mat::zeros(settings->scale*settings->scale,3,CV_32F);
        Mat disVec = Mat::zeros(settings->scale*settings->scale,1,CV_32F);
        GetImVector(img, featureVec, disVec);
        All_vector.push_back(featureVec);
        All_disVector.push_back(disVec);
    }
    //_debugSize(All_vector);
    //_debugSize(All_disVector);
    Mat labels, centers;
    kmeans(All_vector, settings->Bin_num, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), 5, KMEANS_PP_CENTERS, centers);
    //_debugSize(labels);
    //_debugSize(centers);
    Mat Cluster_Map=labels.reshape(0,settings->scale);
    _debugSize(Cluster_Map);
    //_debug(centers);
    Mat Sal_weight_co = GetSalWeight(centers,labels);
    Mat Dis_weight_co = GetPositionW(labels, All_disVector, settings->Bin_num);
    Mat co_weight_co=GetCoWeight(labels);
    //_debug(co_weight_co);
    //_debug(Sal_weight_co);
    //_debug(Dis_weight_co);
    
    double minVal,maxVal;
    minMaxLoc( Sal_weight_co, &minVal, &maxVal);
    //cout << "min " << minVal << " max: " << maxVal << endl;
    Gauss_normal(Sal_weight_co, maxVal, (maxVal-minVal)/2);
    minMaxLoc( Dis_weight_co, &minVal, &maxVal);
    //cout << "min " << minVal << " max: " << maxVal << endl;
    Gauss_normal(Dis_weight_co, maxVal, (maxVal-minVal)/2);
    minMaxLoc( co_weight_co, &minVal, &maxVal);
    //cout << "min " << minVal << " max: " << maxVal << endl;
    Gauss_normal(co_weight_co, maxVal, (maxVal-minVal)/2);
    //_debug(Sal_weight_co);
    //_debug(Dis_weight_co);
    //_debug(co_weight_co);
    Mat SaliencyWeight;
    SaliencyWeight= Sal_weight_co.mul(Dis_weight_co);
    SaliencyWeight= SaliencyWeight.mul(co_weight_co);
    //_debug(SaliencyWeight);
    Mat Saliency_Map_co = Cluster2img(Cluster_Map, SaliencyWeight, settings->Bin_num);

    //bool eq = countNonZero(diff) == 0;
    //cout << eq << endl;
    //_debugSize(test[0]);
    //_debugSize(test[1]);

            //bin_idx.at<float>(i,j)=(float) (cv::countNonZero(img_idx(Rect(j*settings->scale,0,settings->scale, settings->scale))==i))/tmp;
    //_debug(Saliency_Map_co);
    return  Saliency_Map_co;

}

