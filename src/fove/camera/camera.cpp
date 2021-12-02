#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include "camera.h"
#include "./../../gaze/gaze.h"
#include "./../../main.h"

cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;


std::vector<cv::Vec3f> left_circles;
std::vector<cv::Vec3f> right_circles;

FILE* gaze_row_output;
FILE* gaze_output;
bool save = false;


EyeCamera::EyeCamera(void){}
EyeCamera::~EyeCamera(void){}


void EyeCamera::capture(void){
    int count = 0;
    EyeInfoGetterV2 eye_info_getter;
    cv::VideoCapture cap(1);
    if(!cap.isOpened()) return;

    while(cap.read(frame))//無限ループ
    {
        std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(frame);
        cv::Mat pupil_frame = eye_info_getter.draw_pupil_center(frame, pupil_pos);

        cv::imshow("Fove Eyes", pupil_frame);//画像を表示．
        int key = cv::waitKey(1);


        if(key == 'q'/*113*/)//qボタンが押されたとき
        {
            std::cout << frame.size() << std::endl;
            break;//whileループから抜ける．
        }
        else if(key == 's' || save == true)//sが押されたとき
        {
            save = true;
            //フレーム画像を保存する．
            std::stringstream ss_frame;
            //ss_frame << "/share/home/hara/Data/fove/image/" << count << ".png";
            //cv::imwrite(ss_frame.str(), frame);
            count++;
        }
    }
    
    cv::destroyAllWindows();
}

