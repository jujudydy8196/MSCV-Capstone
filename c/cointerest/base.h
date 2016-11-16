#include <iomanip>
#include <sstream>
#include "../config.h"
using namespace std;
using namespace cv;
//config* settings;
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
            struct stat st = {0};
            if (stat(cropFolder.c_str(), &st) == -1) {
                cout << "make dir " << cropFolder << endl;
                mkdir(cropFolder.c_str(), 0777);
            }
            //cout << "done base class" << endl;
        }
    void crop () {
        //cout << "start crop" << endl;
        int w=orgSize.width/gridSize;
        int h=orgSize.height/gridSize;
        for (int i=0; i<img_num; i++) {
            vector<Mat> crop;
            Mat img = data_org_img[i];
            //Mat img = data_image_cv[i];
            string cropFolderWithimg=cropFolder+to_string(i)+"/";
            struct stat st = {0};
            if (stat(cropFolderWithimg.c_str(), &st) == -1) {
                cout << "make dir " << cropFolderWithimg << endl;
                mkdir(cropFolderWithimg.c_str(), 0777);
            }
            for (int ww=0; ww< w ; ww++) {
                for (int hh=0; hh< h; hh++) {
                    //if (i==0 && ww==w-1 && hh==h-1)
                        //continue;
                    stringstream ss;
                    ss << setw(3) << setfill('0') << ww*h+hh;
                    string fileName=ss.str();
                    crop.push_back(img(Rect(ww*gridSize,hh*gridSize,gridSize,gridSize)));
                    imwrite(cropFolderWithimg+fileName+".ppm",img(Rect(ww*gridSize,hh*gridSize,gridSize,gridSize)));
                }
            }
            crops.push_back(crop);
        }
    }
};
