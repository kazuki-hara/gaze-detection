#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <memory>
#include <linux/videodev2.h>
#include "camera.h"


Camera::Camera(std::string dev_name){
    struct stat l_st;
    struct v4l2_capability l_v4l2cap;
    char l_dev_name[1024];
    std::cout << l_dev_name << std::endl;
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
    }
    if(m_fd == -1){
        std::cout << "Cannot open " << dev_name << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "dev_id: " << dev_id << std::endl;
    dev_open = false;
}

Camera::~Camera(void){}


void Camera::capture(void){
    cv::VideoCapture cap(dev_id);
    if(!cap.isOpened()) return;
    cv::Mat _frame;
    while(cap.read(_frame))//無限ループ
    {
        frame = _frame;
        dev_open = true;
    }
}

cv::Mat Camera::capture_one_frame(void){
    cv::VideoCapture cap(dev_id);
    cv::Mat one_frame;
    cap.read(one_frame);
    cap.release();
    return one_frame;
}

bool Camera::check_device_opened(void){
    return dev_open;
}

cv::Mat Camera::get_frame(void){
    return frame;
}

int Camera::get_dev_id(void){
    return dev_id;
}