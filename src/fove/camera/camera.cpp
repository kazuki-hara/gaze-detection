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


#include <cstring>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <memory>
#include <linux/videodev2.h>


cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;


std::vector<cv::Vec3f> left_circles;
std::vector<cv::Vec3f> right_circles;

FILE* gaze_row_output;
FILE* gaze_output;
bool save = false;


EyeCamera::EyeCamera(std::string dev_name){
    struct stat l_st;
    struct v4l2_capability l_v4l2cap;
    char l_dev_name[1024];
    int m_fd;
    for(int i=0; i<63; ++i){
        sprintf(l_dev_name, "/dev/video%d", i);
        m_fd = open(l_dev_name, O_RDWR);
        if(ioctl(m_fd, VIDIOC_QUERYCAP, &l_v4l2cap) == -1){
            continue;
        }
        if(-1 == stat(l_dev_name, &l_st)){
            std::cout << "Cannot indentify " << l_dev_name << std::endl;
            continue;
        }
        if(!S_ISCHR(l_st.st_mode)){
            std::cout << l_dev_name << " is not device" << std::endl;
            continue;
        }
        if(std::string((const char *)l_v4l2cap.card) == dev_name){
            std::cout << "Find " << dev_name << std::endl;
            dev_id = i;
            break;
        }
        std::cout << std::string((const char *)l_v4l2cap.card) << std::endl;
    }
    if(m_fd == -1){
        std::cout << "Cannot open " << "Fove" << std::endl;
        exit(EXIT_FAILURE);
    }
}

EyeCamera::~EyeCamera(void){}


void EyeCamera::capture(void){
    int count = 0;
    EyeInfoGetterV2 eye_info_getter;
    cv::VideoCapture cap(dev_id);
    if(!cap.isOpened()) return;

    while(cap.read(frame))//無限ループ
    {
        std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(frame);
        cv::Mat pupil_frame = eye_info_getter.draw_pupil_center(frame, pupil_pos);
        cv::resize(pupil_frame, pupil_frame, cv::Size(), 2, 2);

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

cv::Mat EyeCamera::get_frame(void){
    return frame;
}
