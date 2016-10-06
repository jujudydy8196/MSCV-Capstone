#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
class config {
    
public:
    // image set
    string img_set_name;
    string dir_name;
    string img_path;
    string result_path;


    // co-saliency parameters
    vector<string> files_list;
    int img_num;
    // image resize scale
    int scale;
    // clustering number on multi-image
    int Bin_num;
    // clustering number on single-image
    int Bin_num_single;

    config(string video) {
        img_set_name = video;
        dir_name = "/home/judy/capstone/DATA/";
        img_path = dir_name + "frame/" + img_set_name +"/";
        result_path = dir_name + "test/" ; //"cosalient/" + img_set_name + "/";
        struct stat st = {0};
        if (stat(result_path.c_str(), &st) == -1) {
                mkdir(result_path.c_str(), 0777);
        }
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

        scale=200;
        Bin_num=min(max(2*img_num,10),30);
        Bin_num_single=6;
    }

};
