#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include "camera.h"
#include "./../gaze/gaze.h"

cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;


std::vector<cv::Vec3f> left_circles;
std::vector<cv::Vec3f> right_circles;

std::tuple<double, double, double, double> get_gaze_info(void){
    double lx,ly, rx, ry;
    if(left_circles.size() == 1 && right_circles.size() ==1){
        lx = (double)left_circles[0][0];
        ly = (double)left_circles[0][1];
        rx = (double)right_circles[0][0];
        ry = (double)right_circles[0][1];
    }else{
        lx = -1;
        ly = -1;
        rx = -1;
        ry = -1;
    }
    return std::make_tuple(lx, ly, rx, ry);
}

std::tuple<cv::Mat, cv::Mat> get_pupil_marked_image(cv::Mat frame){
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

    return std::make_tuple(left_circled_image, right_circled_image);
}


EyeCamera::EyeCamera(const int h, const int w, const char* name):
    height(h),
    width(w),
    dev_name(name)
{
}

EyeCamera::~EyeCamera(void){}


void EyeCamera::capture(void){
    int count = 0;
    cv::VideoCapture cap(1);
    if(!cap.isOpened()) return;
    while(cap.read(frame))//無限ループ
    {
        cv::Mat right;
        cv::Mat left;
        std::tuple<cv::Mat, cv::Mat> images = get_pupil_marked_image(frame);
        left = std::get<0>(images);
        cv::imshow("Fove Eyes", left);//画像を表示．
        int key = cv::waitKey(1);
        if(key == 'q'/*113*/)//qボタンが押されたとき
        {
            std::cout << frame.size() << std::endl;
            break;//whileループから抜ける．
        }
        else if(key == 115/*115*/)//sが押されたとき
        {
            //フレーム画像を保存する．
            std::stringstream ss;
            ss << "./../image/" << count << ".png";
            cv::imwrite(ss.str(), left);
            std::cout << ss.str() << std::endl;
            count++;
        }
    }
    
    cv::destroyAllWindows();
}

