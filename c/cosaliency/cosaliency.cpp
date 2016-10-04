#include <stdio.h>
#include <iostream>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/eigen.hpp>
#include "config.h"
#include <ctime>
//#include "SingleSaliency.h"

using namespace std;
using namespace cv;
using Eigen::MatrixXd;

#define _debug(x) cout << "_debug: " << #x << " : " << x.rows() << "x" << x.cols()  << endl;
config* settings;

void GetImVector(const Mat img, Mat &featureVec, Mat &disVec); 
MatrixXd SingleSaliencyMain(const vector<MatrixXd>);
Mat SingleSaliencyMain(const vector<Mat>);
int main() {
    settings = new config("IMG_5094");
    vector<Mat> data_image_cv;
    for (string fileName : settings->files_list) {
        Mat m=imread((settings->img_path+fileName).c_str());
        //m.convertTo(m, CV_32FC3);
        resize(m,m,Size(settings->scale, settings->scale), INTER_CUBIC);
        data_image_cv.push_back(m);
    }
    SingleSaliencyMain(data_image_cv);
}

//MatrixXd colorspace(const MatrixXd img) {
    //MatrixXd resImg; //RGB->Lab
    //Vector3d v << 0.950456 << 1 << 1.088754 ;

//}
void GetImVector(const Mat img, Mat &featureVec, Mat &disVec) {
    //imshow("img1",img);
    Mat img2;
    cvtColor(img, img2, CV_BGR2Lab);
    img2.convertTo(img2, CV_32FC3);
    //cout << img2.size() << " " << img2.channels() << endl;
    featureVec = img2.reshape(1,settings->scale*settings->scale);
    //cout << featureVec.size() << " " << featureVec.channels() << endl;
    for (int row=0; row<settings->scale; row++) {
        for (int col=0; col<settings->scale; col++ ) {
            disVec.at<float>(row*settings->scale+col)=round(sqrt(pow(row-settings->scale/2,2)+pow(col-settings->scale/2,2)));
        }
    }
    //cout << featureVec << endl << endl;
    //cout << disVec << endl << endl;
}

Mat SingleSaliencyMain(const vector<Mat> data) {
    Mat Saliency_Map_single = Mat::zeros(settings->scale, settings->scale * settings->img_num, CV_32F);
    for (Mat img:data) {
        Mat featureVec= Mat::zeros(settings->scale*settings->scale,3,CV_32F);
        Mat disVec = Mat::zeros(settings->scale*settings->scale,1,CV_32F);
        GetImVector(img, featureVec, disVec);
        break;
    }
}
//MatrixXd SingleSaliencyMain(const vector<MatrixXd> data) {
    //MatrixXd Saliency_Map_single = MatrixXd::Zero(settings->scale, settings->scale * settings->img_num);
    //_debug(Saliency_Map_single);
    //for (MatrixXd img:data) {
        //MatrixXd featureVec, disVec;
        //GetImVector(img, featureVec, disVec);
    //}
    //return Saliency_Map_single ;
//}
