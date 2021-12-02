#include <sstream>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <iostream>
#include <tuple>
#include "gaze.h"
#include "./../utils/utils.h"

double x = 100.0;
double y = 100.0;


int main(void){
    FILE* gaze_output;
    gaze_output = fopen("gaze.txt", "w");
    EyeInfoGetterV2 eye_info_getter;

    const char* image_dir = "/share/home/hara/Data/fove/sample/image/";
    int image_num = count_files_num(image_dir);
    for(int i = 0; i<image_num; i++){
        cv::Mat input_image = cv::imread("/share/home/hara/Data/fove/sample/image/" + std::to_string(i) + ".png");
        

        std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(input_image);
        cv::Mat pupil_frame = eye_info_getter.draw_pupil_center(input_image, pupil_pos);


        double lx = std::get<0>(pupil_pos);
        double ly = std::get<1>(pupil_pos);
        double rx = std::get<2>(pupil_pos);
        double ry = std::get<3>(pupil_pos);
        fprintf(gaze_output, "%f %f %f %f %d\n", lx, ly, rx, ry, i);

        cv::imshow("Fove Eyes", pupil_frame);//画像を表示．


        int key = cv::waitKey(1);
    }
    return 0;
}