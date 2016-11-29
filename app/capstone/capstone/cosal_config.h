//
//  cosal_config.h
//  capstone
//
//  Created by Judy Chang on 11/28/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#ifndef cosal_config_h
#define cosal_config_h
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <dirent.h>
//#include <Eigen/Dense>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#ifndef COSAL_CONFIG_H
#define COSAL_CONFIG_H

#include "config.h"
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
            resize(img,tmp,Size(scale,scale),INTER_CUBIC);
            cosalImgs.push_back(tmp);
        }
    }
    
};
#endif


#endif /* cosal_config_h */
