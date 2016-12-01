//
//  config.h
//  capstone
//
//  Created by Judy Chang on 11/28/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#ifndef config_h
#define config_h

#ifdef __cplusplus
//#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>
//#include <dirent.h>
//#include <chrono>
//#include <Eigen/Dense>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/core/eigen.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#import <opencv2/opencv.hpp>
#import <opencv2/highgui/ios.h>
#endif

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#endif
using namespace std;
using namespace cv;
//using namespace std::chrono;

#define _debugSize(x) cout << "_debugSize: " << #x << " : " << x.size() << endl;
#define _debug(x) cout << "_debug: " << #x << " : " << x << endl << endl;
#define _debugTime(start,end,s) cout << s << " takes " << duration_cast<std::chrono::milliseconds>(end-start).count() << " ms" << endl;

class Config {
public:
    // image set
    string img_set_name;
    string dir_name;
    string img_path;
    string cosal_path;
    string result_path;
    
    vector<string> files_list;
    vector<Mat> data_image_cv;
//    vector<Mat> data_org_img;
    NSMutableArray *imagesArray ;
    int img_num;
    
    cv::Size imsize, orgSize;
    
    Config() {
        cout << "in config" << endl;
        
        img_set_name = "IMG_5094";
        dir_name = "/home/judy/capstone/DATA/";
//        img_path = dir_name + "frame/" + img_set_name +"/";
        result_path = dir_name + "c_result/" + img_set_name +"/";
        
        NSString * resourcePath = [[NSBundle mainBundle] resourcePath];
        NSString * framePath = [resourcePath stringByAppendingPathComponent:@"IMG_5094"];
//        NSLog(framePath);
        NSError *error = nil;
        NSArray *filenames = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:framePath error:&error];
//        NSLog(@"FILES: %@", filenames);
        imagesArray = [[NSMutableArray alloc] init];
        [filenames enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
            NSString *filename = (NSString *)obj;
            NSString *extension = [[filename pathExtension] lowercaseString];
//            NSLog(@"%@", filename);
            if ([extension isEqualToString:@"png"]) {
//                NSLog(@"%@", filename);
                NSString *cname = [framePath stringByAppendingPathComponent:filename];
                UIImage *img = [UIImage imageNamed:cname];
                [imagesArray addObject:img];
                cv::Mat cvImage;
                UIImageToMat(img, cvImage,true);
//                cout << cvImage.size() << endl;
                orgSize=cvImage.size();
//                resize(m,m,Size(),0.5,0.5, INTER_CUBIC);
//                imsize = m.size();
//                cout << cvImage.channels() << endl;
                data_image_cv.push_back(cvImage);
            }
        }];
        img_num = data_image_cv.size();//[imagesArray count];
        cout << img_num << endl;
        
//        if (!fs::exists(result_path)) {
//            fs::create_directories(result_path); // create src folder
//        }
//        
//        cosal_path = dir_name + "c_cosalient/" + img_set_name + "/";
//        if (!fs::exists(cosal_path)) { // Check if src folder exists
//            fs::create_directories(cosal_path); // create src folder
//        }
//        DIR *dir;
//        struct dirent *ent;
//        
//        if ((dir = opendir(img_path.c_str())) != NULL) {
//            while ((ent = readdir(dir)) != NULL) {
//                if (ent->d_type != DT_DIR) {
//                    files_list.push_back(ent->d_name);
//                    //cout << ent->d_name << endl;
//                }
//            }
//            closedir (dir);
//        }
//        else {
//            /* could not open directory */
//            cout << "failed to open the file" << endl;
//        }
//        sort(files_list.begin(),files_list.end());
//        img_num=files_list.size();
//        cout << "total : " << img_num << " imgs" << endl;
//        for (string fileName : files_list) {
//            Mat m=imread((img_path+fileName).c_str(),1);
//            orgSize = m.size();
//            data_org_img.push_back(m);
//            //m.convertTo(m, CV_32FC3);
//            //resize(m,m,Size(scale, scale), INTER_CUBIC);
//            resize(m,m,Size(),0.5,0.5, INTER_CUBIC);
//            imsize = m.size();
//            data_image_cv.push_back(m);
//        }
    }
};
class cosal_config : public Config {
    
public:
    vector<Mat> cosalImgs;
    int scale;
    int Bin_num;
    int Bin_num_single;
    
    cosal_config() {
        
        scale=200;
        Bin_num=min(max(2*img_num,10),30);
        Bin_num_single=6;
        
        for (Mat img:data_image_cv) {
            Mat tmp;
            resize(img,tmp,cv::Size(scale,scale), INTER_CUBIC);
            cosalImgs.push_back(tmp);
        }
    }
    
};

class base: public Config{
public:
    string cropFolder;
    int gridSize;
    int n_gmm, n_pca;
    vector<vector<Mat>> crops;
    base() {
        //cout << "init base class" << endl;
        gridSize=120;
        n_gmm=12;
        n_pca=64;
        //imgFolder="/home/judy/capstone/DATA/frame/";
        cropFolder= dir_name + "c_crop/" + img_set_name + "/";
//        struct stat st = {0};
//        if (stat(cropFolder.c_str(), &st) == -1) {
//            cout << "make dir " << cropFolder << endl;
//            mkdir(cropFolder.c_str(), 0777);
//        }
        //cout << "done base class" << endl;
    }
    void crop () {
        //cout << "start crop" << endl;
        int w=orgSize.width/gridSize;
        int h=orgSize.height/gridSize;
        for (int i=0; i<img_num; i++) {
            vector<Mat> crop;
//            Mat img = data_org_img[i];
            Mat img = data_image_cv[i];
//            string cropFolderWithimg=cropFolder+to_string(i)+"/";
//            struct stat st = {0};
//            if (stat(cropFolderWithimg.c_str(), &st) == -1) {
//                cout << "make dir " << cropFolderWithimg << endl;
//                mkdir(cropFolderWithimg.c_str(), 0777);
//            }
            for (int ww=0; ww< w ; ww++) {
                for (int hh=0; hh< h; hh++) {
                    //if (i==0 && ww==w-1 && hh==h-1)
                    //continue;
//                    stringstream ss;
//                    ss << setw(3) << setfill('0') << ww*h+hh;
//                    string fileName=ss.str();
                    crop.push_back(img(cv::Rect(ww*gridSize,hh*gridSize,gridSize,gridSize)));
//                    imwrite(cropFolderWithimg+fileName+".ppm",img(cv::Rect(ww*gridSize,hh*gridSize,gridSize,gridSize)));
                }
            }
            crops.push_back(crop);
        }
    }
};


#endif /* config_h */
