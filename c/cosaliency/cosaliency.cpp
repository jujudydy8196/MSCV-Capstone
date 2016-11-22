#include <stdio.h>
#include <iostream>
#include "config.h"
#include <time.h>
#include <math.h>
#include <chrono>
//#include "SingleSaliency.h"

using namespace std;
//using namespace cv;
using namespace std::chrono;

#define _debugSize(x) cout << "_debugSize: " << #x << " : " << x.size() << endl;
#define _debug(x) cout << "_debug: " << #x << " : " << x << endl << endl;
#define _debugTime(start,end,s) cout << s << " takes " << duration_cast<std::chrono::milliseconds>(end-start).count() << " ms" << endl;
cosal_config* settings;

void GetImVector(const Mat &img, Mat &featureVec, Mat &disVec); 
Mat GetSalWeight(const Mat &centers, const Mat &labels); 
Mat GetPositionW(const Mat &labels, const Mat &disVec); 
Mat SingleSaliencyMain();
//Mat SingleSaliencyMain(const vector<Mat> &);
void Gauss_normal(Mat &input, float center, float sigma ) ;
Mat Cluster2img(const Mat &Cluster_Map, const Mat &SaliencyWeight_all, const int bin_num) ;
Mat CoSaliencyMain(); 
//Mat CoSaliencyMain(const vector<Mat> &data); 
Mat GetCoWeight( const Mat &labels ); 
void saveResult(const Mat &result); 
//void saveResult(const Mat &result, const Size imsize); 
int main() {
    auto totalstart=high_resolution_clock::now();
    auto start=high_resolution_clock::now();
    settings = new cosal_config();
    auto end=high_resolution_clock::now();
    _debugTime(start,end,"config");
    start = high_resolution_clock::now();
    //vector<Mat> data_image_cv;
    //for (string fileName : settings->files_list) {
        //Mat m=imread((settings->img_path+fileName).c_str(),1);
        //imsize=m.size();
        ////m.convertTo(m, CV_32FC3);
        //resize(m,m,Size(settings->scale, settings->scale), INTER_CUBIC);
        //data_image_cv.push_back(m);
    //}
    end = high_resolution_clock::now();
    _debugTime(start,end,"initialize data");
    start = high_resolution_clock::now();
    Mat result_sig_map = SingleSaliencyMain();
    //Mat result_sig_map = SingleSaliencyMain(settings->data_image_cv);
    end = high_resolution_clock::now();
    _debugTime(start,end,"Single saliency");
    start = high_resolution_clock::now();
    //_debug(result_sig_map);
    //imwrite( "single.png", result_sig_map*255 );
    Mat result_cos_map = CoSaliencyMain();
    //Mat result_cos_map = CoSaliencyMain(settings->data_image_cv);
    end = high_resolution_clock::now();
    _debugTime(start,end,"Co saliency");
    start = high_resolution_clock::now();
    Mat result = result_sig_map.mul(result_cos_map);
    saveResult(result);
    end = high_resolution_clock::now();

    _debugTime(start,end,"save result");

    auto totalend= high_resolution_clock::now();
    _debugTime(totalstart,totalend,"total");
    //_debug(result);
    //imwrite( "cos.png", result_cos_map*255 );
    //imwrite( "result.png", result*255 );
}
void saveResult(const Mat &result) {
//void saveResult(const Mat &result, const Size imsize) {
    for (int i=0; i<settings->img_num; i++) {
        Mat r= result(Rect(settings->scale*i, 0, settings->scale, settings->scale));
        resize(r,r,settings->orgSize, INTER_CUBIC);
        imwrite(settings->cosal_path+settings->files_list[i].substr(0,3)+"_cosaliency.png", r*255);
    }
}
void GetImVector(const Mat &img, Mat &featureVec, Mat &disVec) {
    Mat img2;
    cvtColor(img, img2, CV_BGR2Lab);
    //_debug(img2);
    img2.convertTo(img2, CV_32F);//, 1.0/255.0);
    featureVec = img2.reshape(1,settings->scale*settings->scale);
    featureVec.col(0)= featureVec.col(0).mul(100/255.0);
    featureVec.col(1) -= 128;
    featureVec.col(2) -= 128;
    //_debug(featureVec);
    for (int row=0; row<settings->scale; row++) {
        for (int col=0; col<settings->scale; col++ ) {
            disVec.at<float>(row*settings->scale+col)=round(sqrt(pow((row+1)-settings->scale/2,2)+pow((col+1)-settings->scale/2,2)));
        }
    }
}
Mat GetSalWeight(const Mat &centers, const Mat &labels) {
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
Mat GetCoWeight( const Mat &labels ) {
    int img_num=settings->img_num; //labels.rows/(settings->scale*settings->scale);
    int bin_num=settings->Bin_num; //maxVal;
    Mat img_idx=labels.reshape(0,settings->scale);
    Mat bin_idx= Mat::zeros(bin_num,img_num,CV_32F);
    for (int i=0; i<bin_num; i++) {
        float tmp=cv::countNonZero(img_idx==i);
        for (int j=0; j<img_num; j++) {
        //Saliency_sig_final.copyTo(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
            bin_idx.at<float>(i,j)=(float) (cv::countNonZero(img_idx(Rect(j*settings->scale,0,settings->scale, settings->scale))==i))/tmp;
        }
    }
    Mat co_weight=Mat::zeros(bin_num,1,CV_32F);
    for (int i=0; i<bin_num; i++) {
       bin_idx.row(i) /= sum(bin_idx.row(i))[0];
       Scalar mean, std;
       meanStdDev(bin_idx.row(i),mean,std);
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
        Mat mask= labels==i;
        mask.convertTo(mask, CV_8U, 1.0/255.0);
        Mat disVecIdx;
        disVec.copyTo(disVecIdx, mask);
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
Mat Cluster2img(const Mat &Cluster_Map, const Mat &SaliencyWeight_all, const int bin_num) {
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
    //_debugSize(Saliency_Map_single);
    return Saliency_Map_single;
}
Mat SingleSaliencyMain() {
//Mat SingleSaliencyMain(const vector<Mat> &data) {
    Mat Saliency_Map_single = Mat::zeros(settings->scale, settings->scale * settings->img_num, CV_32F);
    for (int i=0; i<settings->img_num; i++) { //Mat img:data
        Mat img=settings->cosalImgs[i];
        //vector<Mat> ch(3);
        //split(img,ch);
        //_debug(ch[0]);
        //_debug(img);
        Mat featureVec= Mat::zeros(settings->scale*settings->scale,3,CV_32F);
        Mat disVec = Mat::zeros(settings->scale*settings->scale,1,CV_32F);
        GetImVector(img, featureVec, disVec);
        //_debugSize(featureVec);
        //_debug(featureVec);
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
        Saliency_sig_final.copyTo(Saliency_Map_single(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
        //cout << countNonZero(Saliency_Map_single) << endl;
        //_debug(Saliency_Map_single);
        //break;
    }
    return Saliency_Map_single;

}

Mat CoSaliencyMain() {
//Mat CoSaliencyMain(const vector<Mat> &data) {
    Mat All_vector, All_disVector;
    for (int i=0; i<settings->img_num; i++) {
        Mat img=settings->cosalImgs[i];
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
    Mat Cluster_Map=Mat::zeros(settings->scale,settings->scale*settings->img_num,CV_32F);
    for (int i=0; i<settings->img_num; i++) {
        Mat tmp = labels(Rect(0,i*settings->scale*settings->scale,1,settings->scale*settings->scale)).reshape(0,settings->scale);
        tmp.copyTo(Cluster_Map(Rect(i*settings->scale, 0, settings->scale, settings->scale)));
    }
    //Mat Cluster_Map=labels.reshape(0,settings->scale);
    imwrite("cluster.png", Cluster_Map/settings->Bin_num*255);
    //_debugSize(labels);
    Mat Sal_weight_co = GetSalWeight(centers,labels);
    Mat Dis_weight_co = GetPositionW(labels, All_disVector, settings->Bin_num);
    Mat co_weight_co = GetCoWeight(labels);
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
    Mat sal = (Mat_<double>(22,1) << 0.4694,
    0.1636,
    0.1955,
    0.5690,
    0.6900,
    0.1626,
    0.1618,
    0.1353,
    0.3776,
    0.3181,
    0.1556,
    0.3108,
    1.0000,
    0.1428,
    0.4661,
    0.3141,
    0.2466,
    0.3625,
    0.2280,
    0.7679,
    0.5118,
    0.3755);
    Mat dis = (Mat_<double>(22,1) <<     0.1353,
    0.9809,
    0.7550,
    0.9139,
    0.4082,
    0.8571,
    0.8939,
    0.8048,
    1.0000,
    0.9485,
    0.9012,
    0.8236,
    0.3956,
    0.9750,
    0.6949,
    0.7857,
    0.9598,
    0.9750,
    0.9098,
    0.9847,
    0.4726,
    0.8196);
    Mat co= (Mat_<double>(22,1) << 0.1353,
    0.9996,
    1.0000,
    0.4929,
    0.6360,
    0.9825,
    0.9118,
    0.8367,
    0.7725,
    0.5294,
    0.7737,
    0.8333,
    0.7075,
    0.9485,
    0.3711,
    0.9885,
    0.9002,
    0.5302,
    0.8564,
    0.5808,
    0.2903,
    0.4707);
	//_debug(sal);
	//_debug(dis);
    //_debug(co);
    Mat SaliencyWeight;
    //SaliencyWeight= sal.mul(dis);
    //SaliencyWeight= SaliencyWeight.mul(co);
    SaliencyWeight= Sal_weight_co.mul(Dis_weight_co);
    SaliencyWeight= SaliencyWeight.mul(co_weight_co);
    Mat Saliency_Map_co = Cluster2img(Cluster_Map, SaliencyWeight, settings->Bin_num);

    //_debugSize(test[0]);
    //_debugSize(test[1]);

    return  Saliency_Map_co;

}

