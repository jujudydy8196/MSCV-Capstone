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
Mat Cluster2img(const Mat Cluster_Map, const Mat SaliencyWeight_all) ;
int main() {
    settings = new config("IMG_5094");
    vector<Mat> data_image_cv;
    for (string fileName : settings->files_list) {
        Mat m=imread((settings->img_path+fileName).c_str());
        //m.convertTo(m, CV_32FC3);
        resize(m,m,Size(settings->scale, settings->scale), INTER_CUBIC);
        data_image_cv.push_back(m);
    }
    //clock_t begin=clock();
    SingleSaliencyMain(data_image_cv);
    //clock_t end=clock();
    //cout << double(end-begin)/(double)CLOCKS_PER_SEC << endl;
}

void GetImVector(const Mat img, Mat &featureVec, Mat &disVec) {
    //imshow("img1",img);
    Mat img2;
    cvtColor(img, img2, CV_BGR2Lab);
    img2.convertTo(img2, CV_32FC3);
    featureVec = img2.reshape(1,settings->scale*settings->scale);
    for (int row=0; row<settings->scale; row++) {
        for (int col=0; col<settings->scale; col++ ) {
            disVec.at<float>(row*settings->scale+col)=round(sqrt(pow(row-settings->scale/2,2)+pow(col-settings->scale/2,2)));
        }
    }
}
Mat GetSalWeight(const Mat centers, const Mat labels) {
    vector<float> bin_weight(settings->Bin_num_single,0);
    for (int i=0; i<settings->Bin_num_single; i++) {
        bin_weight[i] = (float)cv::countNonZero(labels==i) / labels.rows;
    }
    Mat y = Mat::zeros(settings->Bin_num_single, settings->Bin_num_single, CV_32F);
    for (int i=0; i<settings->Bin_num_single; i++) {
        for (int j=i; j<settings->Bin_num_single; j++) {
            y.at<float>(i,j) = y.at<float>(j,i) = norm(centers.row(i)-centers.row(j));
            y.at<float>(i,j) *= bin_weight[i];
            y.at<float>(j,i) *= bin_weight[j];
        }
    }
    
    Mat Sal_weight;
    reduce(y,Sal_weight,0,CV_REDUCE_SUM, CV_32F);
    return Sal_weight.t();
}
Mat GetPositionW(const Mat labels, const Mat disVec) {
    Mat disWeight = Mat::zeros(settings->Bin_num_single, 1, CV_32F);
    for (int i=0; i<settings->Bin_num_single; i++) {
        Mat mask=labels==i;
        mask.convertTo(mask,CV_8U,1.0/255.0);
        Mat disVecIdx;
        disVec.copyTo(disVecIdx,mask);
        disWeight.at<float>(i)=sum(disVecIdx)[0]/countNonZero(labels==1);

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
Mat Cluster2img(const Mat Cluster_Map, const Mat SaliencyWeight_all) {
    Mat Saliency_sig_temp = Mat::zeros(settings->scale, settings->scale, CV_32F);
    //_debug(SaliencyWeight_all);
    for (int i=0; i<settings->Bin_num_single; i++) {
        Mat mask=Cluster_Map==i;
        mask.convertTo(mask, CV_8U, 1.0/255.0);
        //cout << SaliencyWeight_all.at<float>(i) << endl;
        Saliency_sig_temp.setTo(SaliencyWeight_all.at<float>(i),mask);
    }
    //_debug(Saliency_sig_temp);
    Mat Saliency_Map_single;
    GaussianBlur(Saliency_sig_temp, Saliency_Map_single, Size(3,3), 3);
    //_debug(Saliency_Map_single);
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
        Mat Dis_weight = GetPositionW(labels, disVec);
        
        double minVal,maxVal;
        minMaxLoc( Sal_weight, &minVal, &maxVal);
        Gauss_normal(Sal_weight, maxVal, (maxVal-minVal)/2);
        minMaxLoc( Dis_weight, &minVal, &maxVal);
        Gauss_normal(Dis_weight, maxVal, (maxVal-minVal)/2);
        //_debug(Sal_weight);
        //_debug(Dis_weight);
        //_debug(Sal_weight.mul(Dis_weight));
        Mat SaliencyWeight_all=Sal_weight.mul(Dis_weight);
        Mat Saliency_sig_final=Cluster2img(Cluster_Map, SaliencyWeight_all);
        //_debugSize(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale))); 
        //cout << countNonZero(Saliency_sig_final) << endl;
        //Mat mask = Saliency_Map_single(Range::all(), Range(i*settings->scale, (i+1)*settings->scale-1));
        Saliency_sig_final.copyTo(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
        //cout << countNonZero(Saliency_Map_single) << endl;
        //_debug(Saliency_Map_single);
        //break;
    }

}

