#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <chrono>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/eigen.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
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
        int img_num;

        Size imsize, orgSize;

        Config() {
   
            img_set_name = "IMG_5094";
            dir_name = "/home/judy/capstone/DATA/";
            img_path = dir_name + "frame/" + img_set_name +"/";
            result_path = dir_name + "c_result/" + img_set_name +"/";
	
            if (!fs::exists(result_path)) {
                fs::create_directories(result_path); // create src folder
            }

            cosal_path = dir_name + "c_cosalient/" + img_set_name + "/";
            if (!fs::exists(cosal_path)) { // Check if src folder exists
                fs::create_directories(cosal_path); // create src folder
            }
            //struct stat st = {0};
            //if (stat(cosal_path.c_str(), &st) == -1) {
                    //mkdir(cosal_path.c_str(), 0777);
            //}
            DIR *dir;
            struct dirent *ent;
            
            if ((dir = opendir(img_path.c_str())) != NULL) {
                while ((ent = readdir(dir)) != NULL) {
                    if (ent->d_type != DT_DIR) {
                        files_list.push_back(ent->d_name);
                        //cout << ent->d_name << endl;
                    }
                }
                closedir (dir);
            }
            else {
                /* could not open directory */
                cout << "failed to open the file" << endl;
            }
            sort(files_list.begin(),files_list.end());
            img_num=files_list.size();
            cout << "total : " << img_num << " imgs" << endl;
            for (string fileName : files_list) {
                Mat m=imread((img_path+fileName).c_str(),1);
                orgSize = m.size();
                data_org_img.push_back(m);
                //m.convertTo(m, CV_32FC3);
                //resize(m,m,Size(scale, scale), INTER_CUBIC);
                resize(m,m,Size(),0.5,0.5, INTER_CUBIC);
                imsize = m.size();
                data_image_cv.push_back(m);
            }
        }
};
