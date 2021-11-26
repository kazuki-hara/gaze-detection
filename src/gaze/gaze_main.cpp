#include <sstream>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <iostream>
#include "gaze.h"

#define image_dir "/share/home/hara/Data/fove/image/"

double x = 100.0;
double y = 100.0;


int bfs(std::vector<std::vector <int>> &image, int start_x, int start_y){
    int area = 1;
    //std::cout << map.at<int>(start_y, start_x) << std::endl;
    image[start_y][start_x] = 1;

    int y_max = image.size();
    int x_max = image[0].size();
    std::vector<int> dir({-1,0,1,0,-1});
    for (int i = 0; i < 4; i++) {
        int x = start_x+dir[i], y = start_y+dir[i+1];
        if (0 <= x && x<x_max && 0<=y && y<y_max && image[y][x] == 0){
            area += bfs(image, x, y);
            
        }
    }
    return area;
}

std::tuple<cv::Mat, cv::Mat> get_pupil_marked_image(cv::Mat frame){
    std::vector<cv::Vec3f> left_circles;
    std::vector<cv::Vec3f> right_circles;
    EyeInfoGetter eyeinfo;
    std::tuple<cv::Mat, cv::Mat> images = eyeinfo.convert_image(frame);

    // 左目
    cv::Mat left = std::get<0>(images);
    eyeinfo.preprocess(left);
    left_circles = eyeinfo.make_hough_circle(left);
    cv::Mat left_circled_image = eyeinfo.draw_circles(left, left_circles);

    // 右目
    cv::Mat right = std::get<1>(images);
    eyeinfo.preprocess(right);
    right_circles = eyeinfo.make_hough_circle(right);
    cv::Mat right_circled_image = eyeinfo.draw_circles(right, right_circles);

    //return std::make_tuple(left_circled_image, right_circled_image);
    return std::make_tuple(left_circled_image, right_circled_image);
}



int main(void){
    std::string imgae_dir = "/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/frame/";
    FILE* gaze_output;
    gaze_output = fopen("gaze.txt", "w");
    EyeInfoGetter eyeinfo;


    double lx, ly, rx, ry;
    for(int i = 0; i<=914; i++){
        cv::Mat input_image = cv::imread("/share/home/hara/Data/fove/gaze/old_data/20211015/01/image/frame/" + std::to_string(i) + ".png");
        std::tuple<cv::Mat, cv::Mat> converted_image = eyeinfo.convert_image(input_image);
        cv::Mat left_image = std::get<0>(converted_image);
        cv::Mat right_image = std::get<1>(converted_image);
        cv::Canny(left_image, left_image, CANNY_THRES/2, CANNY_THRES);
        cv::Canny(right_image, right_image, CANNY_THRES/2, CANNY_THRES);
        std::vector<cv::Vec3f> left_circles = eyeinfo.make_hough_circle(left_image);
        std::vector<cv::Vec3f> right_circles = eyeinfo.make_hough_circle(right_image);
        if(left_circles.size() != 0){
            lx = left_circles[0][0];
            ly = left_circles[0][1];
        }
        else{
            lx = -1;
            ly = -1;
        }
        if(right_circles.size() != 0){
            rx = right_circles[0][0];
            ry = right_circles[0][1];
        }
        else{
            rx = -1;
            ry = -1;
        }
        if(lx != -1 && ly != -1 && rx != -1 && ry != -1) fprintf(gaze_output, "%f %f %f %f %d\n", lx, ly, rx, ry, i);
    }
    return 0;
}