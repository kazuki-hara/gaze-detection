#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include "gaze.h"


EyeInfoGetter::EyeInfoGetter(void){}
EyeInfoGetter::~EyeInfoGetter(void){}

// 両目の画像をグレースケール化->左目と右目に分割
std::tuple<cv::Mat, cv::Mat> EyeInfoGetter::convert_image(cv::Mat original_image){
    cv::Mat gray_image;
    cv::cvtColor(original_image, gray_image, CV_BGR2GRAY);
    cv::Mat left_eye_image;
    cv::Mat right_eye_image;
    left_eye_image = cv::Mat(gray_image, cv::Rect(0, 0, 321, gray_image.size().height));
    right_eye_image = cv::Mat(gray_image, cv::Rect(323, 0, gray_image.size().width - 323, gray_image.size().height));
    return std::make_tuple(left_eye_image, right_eye_image);
}

// 二値化->平滑化->エッジ検出->平滑化
void EyeInfoGetter::preprocess(cv::Mat& eye_image){
    cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    cv::Canny(eye_image, eye_image, CANNY_THRES/2, CANNY_THRES);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(SECOND_GAUSS_K_SIZE, SECOND_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
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