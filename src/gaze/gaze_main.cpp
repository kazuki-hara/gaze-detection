#include <sstream>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <iostream>
#include <tuple>
#include "gaze.h"
#include "./../utils/utils.h"


double x = 100.0;
double y = 100.0;


void pupil_output(std::string image_dir, std::string file_path){
    EyeInfoGetterV2 eye_info_getter;
    FILE* pupil_output_file;
    pupil_output_file = fopen(file_path.c_str(), "w");

    int image_num = count_files_num(image_dir);

    for(int i = 0; i<image_num; i++){
        std::cout << i << std::endl;
        cv::Mat input_image = cv::imread(image_dir + std::to_string(i) + ".png");
        std::tuple<double, double, double, double> pupil_center = eye_info_getter.detect_pupil_center_v2(input_image);
        double lx = std::get<0>(pupil_center);
        double ly = std::get<1>(pupil_center);
        double rx = std::get<2>(pupil_center);
        double ry = std::get<3>(pupil_center);
        fprintf(pupil_output_file, "%f %f %f %f %d\n", lx, ly, rx, ry, i);
        
        int key = cv::waitKey(1);
    }
    fclose(pupil_output_file);
}

int main(void){
    std::string image_dir = "/share/home/hara/Data/fove/pupil/hara/200/image/";
    std::string file_path = "/share/home/hara/Data/fove/pupil/hara/200/pupil_ellipse.txt";
    pupil_output(image_dir, file_path);
    return 0;
}