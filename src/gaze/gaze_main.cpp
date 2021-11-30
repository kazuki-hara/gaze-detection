#include <sstream>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <iostream>
#include <tuple>
#include "gaze.h"

#define image_dir "/share/home/hara/Data/fove/image/"

double x = 100.0;
double y = 100.0;


int main(void){
    std::string imgae_dir = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/frame/";
    FILE* gaze_output;
    gaze_output = fopen("gaze.txt", "w");
    EyeInfoGetterV2 eye_info_getter;

    double lx, ly, rx, ry;
    for(int i = 0; i<=2000; i++){
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