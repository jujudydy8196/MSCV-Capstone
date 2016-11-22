//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <dirent.h>
//#include <Eigen/Dense>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/core/eigen.hpp>
//#include <vector>
//#include <string>
//#include <algorithm>
//using namespace std;
//using namespace cv;
#ifndef COSAL_CONFIG_H
#define COSAL_CONFIG_H

#include "../config.h"
class cosal_config : public Config {
    
public:
    // image set
    //string img_set_name;
    //string dir_name;
    //string img_path;


    // co-saliency parameters
    //vector<string> files_list;
    //vector<Mat> data_image_cv;
    vector<Mat> cosalImgs;
    //int img_num;
    // image resize scale
    int scale;
    // clustering number on multi-image
    int Bin_num;
    // clustering number on single-image
    int Bin_num_single;
    //Size imsize;

    cosal_config() {
        //img_set_name = "IMG_5094";
        //dir_name = "/home/judy/capstone/DATA/";
        //img_path = dir_name + "frame/" + img_set_name +"/";
        //DIR *dir;
        //struct dirent *ent;
        
        //if ((dir = opendir(img_path.c_str())) != NULL) {
            //while ((ent = readdir(dir)) != NULL) {
                //if (ent->d_type != DT_DIR) {
                    //files_list.push_back(ent->d_name);
                    ////cout << ent->d_name << endl;
                //}
            //}
            //closedir (dir);
        //}
        //else {
            //[> could not open directory <]
            //cout << "failed to open the file" << endl;
        //}
        //sort(files_list.begin(),files_list.end());
        //img_num=files_list.size();
        //cout << "total : " << img_num << " imgs" << endl;

        scale=200;
        Bin_num=min(max(2*img_num,10),30);
        Bin_num_single=6;

        for (Mat img:data_image_cv) {
            Mat tmp;
            resize(img,tmp,Size(scale,scale),INTER_CUBIC);
            cosalImgs.push_back(tmp);
        }
        //for (string fileName : files_list) {
            //Mat m=imread((img_path+fileName).c_str(),1);
            //imsize=m.size();
            ////m.convertTo(m, CV_32FC3);
            //resize(m,m,Size(scale, scale), INTER_CUBIC);
            //data_image_cv.push_back(m);
        //}
        
    }

};
#endif
