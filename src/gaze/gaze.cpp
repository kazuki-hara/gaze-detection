#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include "gaze.h"

int name = 0;

void remove_blank_line(cv::Mat & original_image){
    for(int x=0; x<10; x++){
        for(int y=0; y<240; y++){
            original_image.at<cv::Vec3b>(y, x+315) = original_image.at<cv::Vec3b>(y, x);
        }
    }
}

void remove_outside_area(cv::Mat& binarized_image){
    int height = binarized_image.rows;
    int width = binarized_image.cols;

    for(int x=0; x<width; x++){
        for(int y=0; y<height; y++){
            if(binarized_image.at<uchar>(y, x) == 0){
                binarized_image.at<uchar>(y, x) = 255;
            }
            else{
                break;
            }
        }
    }
    for(int x=0; x<width; x++){
        for(int y=height-1; y>=0; y--){
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

    for(unsigned int i =0; i< contours.size(); i++){
        double area = cv::contourArea(contours.at(i));
        if(100 < area && area < 1000) contours_subset.push_back(contours.at(i));
    }

    cv::drawContours(dist, contours_subset,-1,cv::Scalar(0),-1);
    return dist;
}


cv::Mat remove_outside_area_v3(cv::Mat binarized_image){
    cv::Mat dist;
    dist = cv::Mat(binarized_image, cv::Rect(35, 12, 230, 212));
    return dist;
}


cv::Mat draw_hough_circles(cv::Mat original_image, std::vector<cv::Vec3f> left_circles, std::vector<cv::Vec3f> right_circles){
    cv::Mat dist = original_image.clone();
    double x, y, radius;
 
    if(left_circles.size() >= 1){
        for(unsigned int i = 0; i<left_circles.size(); i++){
            x = left_circles[i][0];
            y = left_circles[i][1];
            radius = left_circles[i][2];
            cv::circle(dist, cv::Point(x, y), radius, cv::Scalar(0,0,200), 3, 4);
        }
    }
    if(right_circles.size() >= 1){
        for(unsigned int i = 0; i < right_circles.size(); i++){
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

cv::Mat draw_hough_circle(cv::Mat original_image, cv::Vec3f circle){
    cv::Mat dist = original_image.clone();
    double x, y, radius;

    x = circle[0];
    y = circle[1];
    radius = circle[2];
    cv::circle(dist, cv::Point(x, y), radius, cv::Scalar(0,0,200), 3, 4);
    return dist;
}

cv::Mat draw_pupil_edges(cv::Mat input_image, cv::Mat binarized_image, std::vector<cv::Vec3f> circles){
    cv::Mat dist =  input_image.clone();
    
    double hough_circle_center_x, hough_circle_center_y;

    remove_outside_area(binarized_image);

    if(circles.size() >= 1){
        for(unsigned int i = 0; i<circles.size(); i++){
            hough_circle_center_x = (int)circles[i][0];
            hough_circle_center_y = (int)circles[i][1];

            if(binarized_image.at<uchar>(hough_circle_center_y, hough_circle_center_x) == 0){
                int top_x = hough_circle_center_x;
                int top_y = hough_circle_center_y;

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
                for(unsigned int j =0; j < contours.size(); j++){
                    for(unsigned int k=0; k < contours[j].size(); k++){
                        if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                            for(unsigned int l=0; l < contours[j].size(); l++){
                                int x = contours[j][l].x;
                                int y = contours[j][l].y;
                                dist.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,255);
                            }
                        }
                    }
                }
            }
        }
    }
    return dist;
}

cv::Mat draw_pupil_edge(cv::Mat eye_image, int x, int y){
    cv::Mat dist =  eye_image.clone();
    cv::Mat preprocessed_image = eye_image.clone();

    cv::cvtColor(preprocessed_image, preprocessed_image, CV_BGR2GRAY);
    cv::threshold(preprocessed_image, preprocessed_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    remove_outside_area(preprocessed_image);
    cv::GaussianBlur(preprocessed_image, preprocessed_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    cv::threshold(preprocessed_image, preprocessed_image, 100, 255, cv::THRESH_BINARY);

    if(preprocessed_image.at<uchar>(y, x) == 0){
        int top_x = x;
        int top_y = y;

        while(true){
            if(preprocessed_image.at<uchar>(top_y-1, top_x) == 0){
                top_y -= 1;
            }
            else break;
        }
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(preprocessed_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        cv::Moments mu;
        cv::Point2f mc;
        for(unsigned int j =0; j < contours.size(); j++){
            for(unsigned int k=0; k < contours[j].size(); k++){
                if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                    for(unsigned int l=0; l < contours[j].size(); l++){
                        int x = contours[j][l].x;
                        int y = contours[j][l].y;
                        dist.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,255);
                    }
                }
            }
        }
    }
    return dist;
}


EyeInfoGetterV2::EyeInfoGetterV2(void){}
EyeInfoGetterV2::~EyeInfoGetterV2(void){}

// 左目と右目に分割
std::tuple<cv::Mat, cv::Mat> EyeInfoGetterV2::convert_image(cv::Mat original_image){
    remove_blank_line(original_image);
    cv::Mat left_eye_image = cv::Mat(original_image, cv::Rect(35, 12, 230, 200));
    cv::Mat right_eye_image= cv::Mat(original_image, cv::Rect(360, 12, 230, 200));
    return std::make_tuple(left_eye_image, right_eye_image);
}

// グレースケール変換->二値化-> 外側除去 -> 平滑化->エッジ検出->平滑化
void EyeInfoGetterV2::preprocess(cv::Mat& eye_image){
    cv::cvtColor(eye_image, eye_image, CV_BGR2GRAY);
    //cv::imwrite("1.png", eye_image);
    cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    //cv::imwrite("2.png", eye_image);
    remove_outside_area(eye_image);
    //cv::imwrite("3.png", eye_image);
    cv::GaussianBlur(eye_image, eye_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    //cv::imwrite("4.png", eye_image);
    cv::Canny(eye_image, eye_image, CANNY_THRES/2, CANNY_THRES);
    //cv::imwrite("5.png", eye_image);
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


int EyeInfoGetterV2::select_pupil_circle(cv::Mat input_image, std::vector<cv::Vec3f> circles){
    double index = -1;
    if(circles.size() >= 1){
        int matching_image_size = 40;
        cv::Mat matching_image = cv::Mat::zeros(matching_image_size, matching_image_size, CV_8UC1);
        for(int y = 0; y < matching_image_size; y++){
            for(int x = 0; x < matching_image_size; x++){
                matching_image.at<uchar>(y, x) = 255;
            }
        }
        cv::circle(matching_image, cv::Point(matching_image_size/2, matching_image_size/2), 15, 0, -1, CV_AA);

        cv::Mat eye_image = input_image.clone();

        cv::cvtColor(eye_image, eye_image, CV_BGR2GRAY);
        cv::threshold(eye_image, eye_image, BINARY_THRES, 255, cv::THRESH_BINARY);
        cv::GaussianBlur(eye_image, eye_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
        
        double score = -1;
        for(unsigned int i = 0; i < circles.size(); i++){
            int x = (int)circles[i][0];
            int y = (int)circles[i][1];
            if(matching_image_size/2 <= x && x < eye_image.size().width - matching_image_size/2 && matching_image_size/2 <= y && y < eye_image.size().height - matching_image_size/2){
                cv::Mat eye_image_around_circle = cv::Mat(eye_image, cv::Rect(x - matching_image_size/2, y - matching_image_size/2, 40, 40));
                cv::Mat result_image;
                cv::matchTemplate(eye_image_around_circle, matching_image, result_image, CV_TM_CCOEFF_NORMED);
                double score_tmp = 0;
                cv::Point max_pt;
                cv::minMaxLoc(result_image, NULL, &score_tmp, NULL, &max_pt);
                if(score_tmp > score){
                    score = score_tmp;
                    index = i;
                }
            }
        }
    }
    return index;
}

// 二値化画像から瞳孔重心を算出
std::tuple<double, double> EyeInfoGetterV2::cal_center_of_gravity(cv::Mat binarized_image, std::vector<cv::Vec3f> circles){
    double hough_circle_center_x, hough_circle_center_y;
    double center_x = -1;
    double center_y = -1;

    remove_outside_area(binarized_image);

    if(circles.size() >= 1){
        for(unsigned int i = 0; i<circles.size(); i++){
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
                for(unsigned int j =0; j < contours.size(); j++){
                    for(unsigned int k=0; k < contours[j].size(); k++){
                        if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                            area = cv::contourArea(contours.at(j));
                            int x_min = binarized_image.size().width;
                            int x_max = 0;
                            int y_min = binarized_image.size().height;
                            int y_max = 0;
                            for(unsigned int l=0; l < contours[j].size(); l++){
                                int x = contours[j][l].x;
                                int y = contours[j][l].y;
                                x_min = std::min(x, x_min);
                                x_max = std::max(x, x_max);
                                y_min = std::min(y, y_min);
                                y_max = std::max(y, y_max);
                            }
                            width = x_max - x_min;
                            height = y_max - y_min;
                            //std::cout << width << " " << height << std::endl;

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


std::tuple<double, double> EyeInfoGetterV2::cal_center_of_gravity_v2(cv::Mat eye_image, cv::Vec3f circle){
    double hough_circle_center_x = circle[0];
    double hough_circle_center_y = circle[1];

    cv::Mat preprocessed_image = eye_image.clone();

    cv::cvtColor(preprocessed_image, preprocessed_image, CV_BGR2GRAY);
    cv::threshold(preprocessed_image, preprocessed_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    remove_outside_area(preprocessed_image);
    cv::GaussianBlur(preprocessed_image, preprocessed_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    cv::threshold(preprocessed_image, preprocessed_image, BINARY_THRES, 255, 0);

    if(preprocessed_image.at<uchar>(hough_circle_center_y, hough_circle_center_x) < 255){
        int top_x = hough_circle_center_x;
        int top_y = hough_circle_center_y;

        while(true){
            if(preprocessed_image.at<uchar>(top_y-1, top_x) == 0){
                top_y -= 1;
            }
            else break;
        }
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(preprocessed_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        cv::Moments mu;
        cv::Point2f mc;
        for(unsigned int j =0; j < contours.size(); j++){
            for(unsigned int k=0; k < contours[j].size(); k++){
                if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                    mu = cv::moments(contours[j]);
                    if(mu.m00 != 0){
                        mc = cv::Point2f(mu.m10/mu.m00, mu.m01/mu.m00);
                        return std::make_tuple((double)mc.x, (double)mc.y);
                    }
                }
            }
        }
    }
    return std::make_tuple(-1, -1);
}

std::tuple<bool, cv::RotatedRect> EyeInfoGetterV2::pupil_center_ellipse(cv::Mat eye_image, cv::Vec3f circle){
    double hough_circle_center_x = circle[0];
    double hough_circle_center_y = circle[1];

    cv::Mat preprocessed_image = eye_image.clone();

    cv::cvtColor(preprocessed_image, preprocessed_image, CV_BGR2GRAY);
    cv::threshold(preprocessed_image, preprocessed_image, BINARY_THRES, 255, cv::THRESH_BINARY);
    remove_outside_area(preprocessed_image);
    cv::GaussianBlur(preprocessed_image, preprocessed_image, cv::Size(FIRST_GAUSS_K_SIZE, FIRST_GAUSS_K_SIZE), GAUSS_X_SIGMA, GAUSS_Y_SIGMA);
    //cv::imwrite("blur.png", preprocessed_image);
    cv::threshold(preprocessed_image, preprocessed_image, 100, 255, cv::THRESH_BINARY);
    //cv::imshow("preprocessed", preprocessed_image);
    //cv::imwrite("6.png", preprocessed_image);

    cv::RotatedRect box;
    if(preprocessed_image.at<uchar>(hough_circle_center_y, hough_circle_center_x) < 255){
        int top_x = hough_circle_center_x;
        int top_y = hough_circle_center_y;

        while(true){
            if(preprocessed_image.at<uchar>(top_y-1, top_x) == 0){
                top_y -= 1;
            }
            else break;
        }
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(preprocessed_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        for(unsigned int j =0; j < contours.size(); j++){
            for(unsigned int k=0; k < contours[j].size(); k++){
                if(contours[j][k].y == top_y-1 && contours[j][k].x == top_x){
                    // 楕円フィッティング
                    if(contours[j].size() >=5){
                        box = cv::fitEllipse(contours[j]);
                        return std::make_tuple(true, box);
                    }
                }
            }
        }
    }
    return std::make_tuple(false, box);
}


// 瞳孔位置検出一連の関数
std::tuple<double, double, double, double> EyeInfoGetterV2::detect_pupil_center(cv::Mat original_image){
    // 左目と右目に分割したグレースケール画像を取得
    std::tuple<cv::Mat, cv::Mat> converted_images = convert_image(original_image);
    cv::Mat left_image = std::get<0>(converted_images);
    cv::Mat right_image = std::get<1>(converted_images);
    cv::Mat left_image_copy = left_image.clone();
    cv::Mat right_image_copy = right_image.clone();
    preprocess(left_image_copy);
    preprocess(right_image_copy);

    // ハフ変換による円検出
    std::vector<cv::Vec3f> left_circles = detect_pupil_circle(left_image_copy);
    std::vector<cv::Vec3f> right_circles = detect_pupil_circle(right_image_copy);

    // テンプレートマッチングによる円判定
    int left_pupil_circle_index = select_pupil_circle(left_image, left_circles);
    int right_pupil_circle_index = select_pupil_circle(right_image, right_circles);

    // 左目の瞳孔中心計算
    double lx = -1, ly = -1;
    cv::Mat left_ellipse_image = left_image.clone();
    if(left_pupil_circle_index != -1){
        std::tuple<double, double> left_pupil_center = cal_center_of_gravity_v2(left_image, left_circles[left_pupil_circle_index]);
        lx = std::get<0>(left_pupil_center);
        ly = std::get<1>(left_pupil_center);
        //if(lx == -1 || ly == -1) std::cout << "left circle is not pupil" << std::endl;
        cv::Mat left_edge_image = draw_pupil_edge(left_image, lx, ly);
        cv::imwrite("left_edge.png", left_edge_image);
    }//else std::cout << "left circle is not found" << std::endl;

    // 右目の瞳孔中心計算
    double rx = -1, ry = -1;
    if(right_pupil_circle_index != -1){
        std::tuple<double, double> right_pupil_center = cal_center_of_gravity_v2(right_image, right_circles[right_pupil_circle_index]);
        rx = std::get<0>(right_pupil_center);
        ry = std::get<1>(right_pupil_center);
        //if(lx == -1 || ly == -1) std::cout << "right circle is not pupil" << std::endl;
        cv::Mat edge_image = draw_pupil_edge(right_image, rx, ry);
        cv::imwrite("right_edge.png", edge_image);
    }//else std::cout << " right circle is not found" << std::endl;

    return std::make_tuple(lx, ly, rx, ry);
}


std::tuple<double, double, double, double> EyeInfoGetterV2::detect_pupil_center_v2(cv::Mat original_image){
    // 左目と右目に分割したグレースケール画像を取得
    std::tuple<cv::Mat, cv::Mat> converted_images = convert_image(original_image);
    cv::Mat left_image = std::get<0>(converted_images);
    cv::Mat right_image = std::get<1>(converted_images);
    //cv::imwrite("1.png", right_image);
    cv::Mat left_image_copy = left_image.clone();
    cv::Mat right_image_copy = right_image.clone();
    preprocess(left_image_copy);
    preprocess(right_image_copy);
    //cv::imwrite("2.png", right_image_copy);
    std::vector<cv::Vec3f> left_circles = detect_pupil_circle(left_image_copy);
    std::vector<cv::Vec3f> right_circles = detect_pupil_circle(right_image_copy);

    //cv::Mat circles_image = draw_hough_circles(right_image, right_circles, left_circles);
    //cv::imwrite("3.png", circles_image);

    int index = select_pupil_circle(left_image, left_circles);
    
    double lx = -1, ly = -1, rx = -1, ry = -1;
    l_x = -1, l_y = -1, r_x = -1, r_y = -1;
    if(index != -1){
        std::tuple<bool, cv::RotatedRect> pupil_info = pupil_center_ellipse(left_image, left_circles[index]);
        bool success = std::get<0>(pupil_info);
        if(success){
            cv::RotatedRect box = std::get<1>(pupil_info);
            left_box = box;
            l_x = box.center.x;
            l_y = box.center.y;
            lx = box.center.x;
            ly = box.center.y;
            cv::ellipse(left_image, left_box, cv::Scalar(0, 0, 255), 1, CV_AA);
            cv::imwrite("left_image.png", left_image);
            //cv::imshow("left", left_image);
        }else{
            std::cout << "circle is not pupil" << std::endl;
        }
    }else{
        std::cout << "circle not found" << std::endl;
    }
    index = select_pupil_circle(right_image, right_circles);

    if(index != -1){
        std::tuple<bool, cv::RotatedRect> pupil_info = pupil_center_ellipse(right_image, right_circles[index]);
        bool success = std::get<0>(pupil_info);
        if(success){
            cv::RotatedRect box = std::get<1>(pupil_info);
            right_box = box;
            r_x = box.center.x;
            r_y = box.center.y;
            rx = box.center.x;
            ry = box.center.y;
            //cv::imshow("right", right_image);
        }else{
            std::cout << "circle is not pupil" << std::endl;
        }
    }else{
        std::cout << "circle not found" << std::endl;
    }
    //dist = cv::converted_images(left_image, right_image);
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

std::tuple<cv::RotatedRect, cv::RotatedRect> EyeInfoGetterV2::get_pupil_info(void){
    printf("%lf %lf %lf %lf\n", l_x ,l_y, r_x, r_y);
    return std::make_tuple(left_box, right_box);
}
