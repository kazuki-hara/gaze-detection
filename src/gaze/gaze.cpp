#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include "gaze.h"

bool save_preprocess_image = false;

void remove_blank_line(cv::Mat & image){
    cv::Vec3b color = image.at<cv::Vec3b>(0, 0);
    for(int x=0; x<10; x++){
        for(int y=0; y<240; y++){
            image.at<cv::Vec3b>(y, x+315) = image.at<cv::Vec3b>(y, x);
        }
    }
}


EyeInfoGetter::EyeInfoGetter(void){}
EyeInfoGetter::~EyeInfoGetter(void){}

// 両目の画像をグレースケール化->左目と右目に分割
std::tuple<cv::Mat, cv::Mat> EyeInfoGetter::convert_image(cv::Mat original_image){
    cv::Mat gray_image;
    remove_blank_line(original_image);
    cv::cvtColor(original_image, gray_image, CV_BGR2GRAY);
    cv::Mat left_eye_image;
    cv::Mat right_eye_image;
    left_eye_image = cv::Mat(gray_image, cv::Rect(0, 0, 320, gray_image.size().height));
    right_eye_image = cv::Mat(gray_image, cv::Rect(320, 0, gray_image.size().width - 320, gray_image.size().height));
    return std::make_tuple(left_eye_image, right_eye_image);
}

// 二値化->平滑化->エッジ検出->平滑化
void EyeInfoGetter::preprocess(cv::Mat& eye_image){
    cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    if(save_preprocess_image == true) cv::imwrite("threshold.png", eye_image);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    if(save_preprocess_image == true) cv::imwrite("blur1.png", eye_image);
    cv::Canny(eye_image, eye_image, CANNY_THRES/2, CANNY_THRES);
    if(save_preprocess_image == true) cv::imwrite("canny.png", eye_image);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(SECOND_GAUSS_K_SIZE, SECOND_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    if(save_preprocess_image == true) cv::imwrite("blur2.png", eye_image);
}

// 画像から円を検出
std::vector<cv::Vec3f> EyeInfoGetter::make_hough_circle(cv::Mat eye_image){
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(eye_image, circles, CV_HOUGH_GRADIENT, 1, 
            HOUGH_MIN_CENTER_DIST, CANNY_THRES,
            HOUGH_VOTE, HOUGH_MIN_RADIUS, HOUGH_MAX_RADIUS);
    return circles;
}

// 検出した円を目の画像上に描写
cv::Mat EyeInfoGetter::draw_circles(cv::Mat eye_image, std::vector<cv::Vec3f> circles){
    cv::Mat color_image;
    cv::cvtColor(eye_image, color_image, CV_GRAY2RGB);
    for(unsigned int i= 0; i<circles.size(); i++){
        int x = (int)circles[0][0];
        int y = (int)circles[0][1];
        int radius = (int)circles[0][2];
        cv::circle(color_image, cv::Point(x, y), radius, cv::Scalar(0, 255, 0),2,  -1);
    }
    return color_image;
}

void get_pupil_center(double hough_center_x, double hough_center_y, cv::Mat eye_image){
    double pupil_center_x, pupil_center_y;

    int start_x = (int)hough_center_x;
    int start_y = (int)hough_center_y;

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(eye_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    std::cout << contours.size();
}


EyeInfoGetterV2::EyeInfoGetterV2(void){}
EyeInfoGetterV2::~EyeInfoGetterV2(void){}

// 両目の画像をグレースケール化->左目と右目に分割
std::tuple<cv::Mat, cv::Mat> EyeInfoGetterV2::convert_image(cv::Mat original_image){
    cv::Mat gray_image;
    remove_blank_line(original_image);
    cv::cvtColor(original_image, gray_image, CV_BGR2GRAY);
    cv::Mat left_eye_image;
    cv::Mat right_eye_image;
    left_eye_image = cv::Mat(gray_image, cv::Rect(0, 0, 320, gray_image.size().height));
    right_eye_image = cv::Mat(gray_image, cv::Rect(320, 0, gray_image.size().width - 320, gray_image.size().height));
    return std::make_tuple(left_eye_image, right_eye_image);
}

// 二値化->平滑化->エッジ検出->平滑化
void EyeInfoGetterV2::preprocess(cv::Mat& eye_image){
    cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    cv::Canny(eye_image, eye_image, CANNY_THRES/2, CANNY_THRES);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(SECOND_GAUSS_K_SIZE, SECOND_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
}

// 画像から円を検出
std::vector<cv::Vec3f> EyeInfoGetterV2::detect_pupil_circle(cv::Mat eye_image){
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(eye_image, circles, CV_HOUGH_GRADIENT, 1, 
            HOUGH_MIN_CENTER_DIST, CANNY_THRES,
            HOUGH_VOTE, HOUGH_MIN_RADIUS, HOUGH_MAX_RADIUS);
    return circles;
}

// 二値化画像から瞳孔重心を算出
std::tuple<double, double> EyeInfoGetterV2::cal_center_of_gravity(cv::Mat binarized_image, std::vector<cv::Vec3f> circles){
    double hough_circle_center_x, hough_circle_center_y;
    if(circles.size() >= 1){
        hough_circle_center_x = (double)circles[0][0];
        hough_circle_center_y = (double)circles[0][1];
    }
    else{
        hough_circle_center_x = -1;
        hough_circle_center_y = -1;
    }
    return std::make_tuple(hough_circle_center_x, hough_circle_center_y);
}

// 瞳孔位置検出一連の関数
std::tuple<double, double, double, double> EyeInfoGetterV2::detect_pupil_center(cv::Mat original_image){
    // 左目と右目に分割したグレースケール画像を取得
    std::tuple<cv::Mat, cv::Mat> converted_gray_images = convert_image(original_image);
    cv::Mat left_gray_image = std::get<0>(converted_gray_images);
    cv::Mat right_gray_image = std::get<1>(converted_gray_images);

    // 左目の瞳孔位置検出
    cv::Mat left_binarized_image;
    cv::threshold(left_gray_image, left_binarized_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    preprocess(left_gray_image);
    std::vector<cv::Vec3f> left_circles = detect_pupil_circle(left_gray_image);
    std::tuple<double, double> left_center = cal_center_of_gravity(left_binarized_image, left_circles);
    double lx = std::get<0>(left_center);
    double ly = std::get<1>(left_center);
    l_x = lx;
    l_y = l_y;


    // 右目の瞳孔位置検出
    cv::Mat right_binarized_image;
    cv::threshold(right_gray_image, right_binarized_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    preprocess(right_gray_image);
    std::vector<cv::Vec3f> right_circles = detect_pupil_circle(right_gray_image);
    std::tuple<double, double> right_center = cal_center_of_gravity(right_binarized_image, right_circles);
    double rx = std::get<0>(right_center);
    double ry = std::get<1>(right_center);
    r_x = rx;
    r_y = ry;

    return std::make_tuple(lx, ly, rx, ry);
}


cv::Mat EyeInfoGetterV2::draw_pupil_center(cv::Mat original_image, std::tuple<double, double, double, double> pupil_pos){
    cv::Mat dist = original_image.clone();
    double lx = std::get<0>(pupil_pos);
    double ly = std::get<1>(pupil_pos);
    double rx = std::get<2>(pupil_pos);
    double ry = std::get<3>(pupil_pos);
    if(lx != -1 && ly != -1 && rx != -1 && ry != -1){
        cv::circle(dist, cv::Point(lx, ly), 10, cv::Scalar(0,255,0), -1, CV_AA);
        cv::circle(dist, cv::Point(rx+320, ry), 10, cv::Scalar(0,255,0), -1, CV_AA);
    }
    
    return dist;
};

void EyeInfoGetterV2::get_pupil_info(void){
    printf("%lf %lf %lf %lf\n" , l_x ,l_y, r_x, r_y);
}