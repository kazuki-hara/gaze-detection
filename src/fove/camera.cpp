#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include "camera.h"
#include "./../gaze/gaze.h"
#include "./../main.h"

cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;


std::vector<cv::Vec3f> left_circles;
std::vector<cv::Vec3f> right_circles;

FILE* gaze_row_output;
FILE* gaze_output;
bool save = false;

std::tuple<double, double, double, double> get_gaze_info(void){
    double lx,ly, rx, ry;
    /*
    if(left_circles.size() == 1 && right_circles.size() ==1){
        lx = (double)left_circles[0][0];
        ly = (double)left_circles[0][1];
        rx = (double)right_circles[0][0];
        ry = (double)right_circles[0][1];
    }else{
        std::cout << left_circles.size() << right_circles.size() << std::endl;
        lx = -1;
        ly = -1;
        rx = -1;
        ry = -1;
    }
    */
    if(left_circles.size() >= 1 && right_circles.size() >=1){
        lx = (double)left_circles[0][0];
        ly = (double)left_circles[0][1];
        rx = (double)right_circles[0][0];
        ry = (double)right_circles[0][1];
    }
    else{
        std::cout << left_circles.size() << right_circles.size() << std::endl;
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
    gaze_row_output = fopen("/share/home/hara/workspace/fove/data/gaze/gaze_row_data.txt", "w");
    gaze_output = fopen("/share/home/hara/workspace/fove/data/gaze/gaze.txt", "w");
    int count = 0;
    double lx=0,ly=0,rx=0,ry=0;
    cv::VideoCapture cap(1);
    if(!cap.isOpened()) return;
    while(cap.read(frame))//無限ループ
    {
        cv::Mat right;
        cv::Mat left;
        std::tuple<cv::Mat, cv::Mat> images = get_pupil_marked_image(frame);
        left = std::get<0>(images);
        right = std::get<1>(images);

        std::tuple<double, double, double, double> gaze_info = get_gaze_info();
        lx = std::get<0>(gaze_info);
        ly = std::get<1>(gaze_info);
        rx = std::get<2>(gaze_info);
        ry = std::get<3>(gaze_info);


        //cv::imshow("Fove Eyes", left);//画像を表示．
        int key = cv::waitKey(1);

        if(key == 'q'/*113*/)//qボタンが押されたとき
        {
            std::cout << frame.size() << std::endl;
            break;//whileループから抜ける．
        }
        else if(key == 's' || save == true)//sが押されたとき
        {
            save = true;
            if(lx != -1 && ly != -1 && rx != -1 && ry != -1){
                std::cout << lx << " " << ly << " " << rx << " " << ry << " " << get_passed_time() << std::endl;
                fprintf(gaze_output, "%f %f %f %f %f\n", lx, ly, rx, ry, get_passed_time());
            }
            fprintf(gaze_row_output, "%f %f %f %f %f\n", lx, ly, rx, ry, get_passed_time());

            //フレーム画像を保存する．
            std::stringstream ss_frame;
            ss_frame << "/share/home/hara/workspace/fove/data/image/frame/" << count << ".png";
            cv::imwrite(ss_frame.str(), frame);

            std::stringstream ss_right;
            ss_right << "/share/home/hara/workspace/fove/data/image/right/" << count << ".png";
            cv::imwrite(ss_right.str(), right);

            std::stringstream ss_left;
            ss_left << "/share/home/hara/workspace/fove/data/image/left/" << count << ".png";
            cv::imwrite(ss_left.str(), left);
            count++;
        }
        if(get_passed_time() > 40){
            fclose(gaze_output);
            fclose(gaze_row_output);
            save=false;
        }
    }
    
    cv::destroyAllWindows();
}

