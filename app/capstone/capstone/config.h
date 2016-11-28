//
//  config.h
//  capstone
//
//  Created by Judy Chang on 11/28/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#ifndef config_h
#define config_h
//#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>
//#include <dirent.h>
#include <chrono>
//#include <Eigen/Dense>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/core/eigen.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>

//#include <boost/filesystem.hpp>

//namespace fs = boost::filesystem;
using namespace std;
using namespace cv;
using namespace std::chrono;

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
    vector<Mat> data_org_img;
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
                cv::Mat cvImage; UIImageToMat(img, cvImage);
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


#endif /* config_h */
