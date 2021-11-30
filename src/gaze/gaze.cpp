#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include "gaze.h"

int name = 0;

void remove_blank_line(cv::Mat & image){
    for(int x=0; x<10; x++){
        for(int y=0; y<240; y++){
            image.at<cv::Vec3b>(y, x+315) = image.at<cv::Vec3b>(y, x);
        }
    }
}

void remove_outside_area(cv::Mat& binarized_image){
    int height = binarized_image.rows;
    int width = binarized_image.cols;

    int y_top = 0;
    int y_bottom = height - 1;
    bool inside = false;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(binarized_image.at<uchar>(y, x) == 0){
                binarized_image.at<uchar>(y, x) = 255;
            }
            else{
                inside = true;
                break;
            }
        }
    }
    for(int y=0; y<height; y++){
        for(int x=width-1; x>=0; x--){
            if(binarized_image.at<uchar>(y, x) == 0){
                binarized_image.at<uchar>(y, x) = 255;
            }
            else break;
        }
    }
}

cv::Mat remove_outside_area_v2(cv::Mat binarized_image){
    cv::Mat dist(binarized_image.rows, binarized_image.cols, CV_8UC1, cv::Scalar(255));

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binarized_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    std::vector<std::vector<cv::Point>> contours_subset;

    for(int i =0; i< contours.size(); i++){
        double area = cv::contourArea(contours.at(i));
        if(100 < area && area < 1000) contours_subset.push_back(contours.at(i));
    }

    cv::drawContours(dist, contours_subset,-1,cv::Scalar(0),-1);
    return dist;
}


cv::Mat draw_hough_circles(cv::Mat original_image, std::vector<cv::Vec3f> left_circles, std::vector<cv::Vec3f> right_circles){
    cv::Mat dist = original_image.clone();
    double x, y, radius;
 
    if(left_circles.size() >= 1){
        for(int i = 0; i<left_circles.size(); i++){
            x = left_circles[i][0];
            y = left_circles[i][1];
            radius = left_circles[i][2];
            cv::circle(dist, cv::Point(x, y), radius, cv::Scalar(0,0,200), 3, 4);
        }
    }
    if(right_circles.size() >= 1){
        for(int i = 0; i<right_circles.size(); i++){
            double x = right_circles[i][0];
            double y = right_circles[i][1];
            double radius = right_circles[i][2];
            cv::circle(dist, cv::Point(x+320, y), radius, cv::Scalar(0,0,200), 3, 4);
        }
    }
    //cv::imwrite("images/" + std::to_string(name) + ".png", dist);
    name += 1;
    //cv::imshow("hough", dist);
    //int key = cv::waitKey(1);
    return dist;
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

// 二値化-> 外側除去 -> 平滑化->エッジ検出->平滑化
void EyeInfoGetterV2::preprocess(cv::Mat& eye_image){
    cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    remove_outside_area(eye_image);
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
    double center_x = -1;
    double center_y = -1;

    if(circles.size() >= 1){
        for(int i = 0; i<circles.size(); i++){
            hough_circle_center_x = (int)circles[i][0];
            hough_circle_center_y = (int)circles[i][1];

            if(binarized_image.at<uchar>(hough_circle_center_y, hough_circle_center_x) == 0){
                int top_x = hough_circle_center_x;
                int top_y = hough_circle_center_y;

                int area = 0;
                int width = 0;
                int height = 0;

                while(true){
                    if(binarized_image.at<uchar>(top_y-1, top_x) == 0){
                        top_y -= 1;
                    }
                    else break;
                }
                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(binarized_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                cv::Moments mu;
                cv::Point2f mc;
                for(int j =0; j< contours.size(); j++){
                    for(int k=0; k < contours[j].size(); k++){
                        if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                            area = cv::contourArea(contours.at(j));
                            int x_min = 320;
                            int x_max = 0;
                            int y_min = 240;
                            int y_max = 0;
                            for(int l=0; l < contours[j].size(); l++){
                                int x = contours[j][l].x;
                                int y = contours[j][l].y;
                                x_min = std::min(x, x_min);
                                x_max = std::max(x, x_max);
                                y_min = std::min(y, y_min);
                                y_max = std::max(y, y_max);
                            }
                            width = x_max - x_min;
                            height = y_max - y_min;

                            mu = cv::moments(contours[j]);
                            mc = cv::Point2f(mu.m10/mu.m00, mu.m01/mu.m00);

                            if(100 < area && area < 1500 && std::abs(width - height) < 20){
                                center_x = (double)mc.x;
                                center_y = (double)mc.y;
                                return std::make_tuple(center_x, center_y);
                            }
                            break;
                        }
                    }
                }
            }
        }
        center_x = -1;
        center_y = -1;
    }
    else{
        center_x = -1;
        center_y = -1;
    }
    return std::make_tuple(center_x, center_y);
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
    l_y = ly;


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
    draw_hough_circles(original_image, left_circles, right_circles);

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


