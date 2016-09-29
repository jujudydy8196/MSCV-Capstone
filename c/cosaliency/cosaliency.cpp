#include <stdio.h>
#include <iostream>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/eigen.hpp>
#include "config.h"
#include "SingleSaliency.h"

using namespace std;
using namespace cv;
using Eigen::MatrixXd;

int main() {
    config* settings = new config("IMG_5094");
    vector<MatrixXd> data_image;
    for (string fileName : settings->files_list) {
        Mat m=imread((settings->img_path+fileName).c_str());
        MatrixXd img;
        cv2eigen(m,img);
        data_image.push_back(img);
        //cout << img.rows() << "x" << img.cols() << endl;
    }
    SingleSaliencyMain(data_image, settings.img_num, settings.scale, settings.Big_num);
}

