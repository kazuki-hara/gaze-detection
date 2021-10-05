#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include "camera.h"

cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;
double ratio = 0.5;


EyeCamera::EyeCamera(const int h, const int w, const char* name):
    height(h),
    width(w),
    dev_name(name)
{
}

EyeCamera::~EyeCamera(void){}


void EyeCamera::capture(void){
    int i = 1;
    int save = 0;
    cv::VideoCapture cap(1);
    if(!cap.isOpened()) return;
    cvNamedWindow( "Window" );
    while(cap.read(frame))//無限ループ
    {
        cv::imshow("Fove Eyes", frame);//画像を表示．
        const int key = cv::waitKey(1);
        if(key == 'q'/*113*/)//qボタンが押されたとき
        {
            std::cout << frame.size() << std::endl;
            break;//whileループから抜ける．
        }
        else if(key == 115/*115*/)//sが押されたとき
        {
            //フレーム画像を保存する．
            save = 1;
            std::cout << "start saving" << std::endl;
        }
        if(true){
            std::stringstream ss;
            ss << "./../image/" << i << ".png";
            cv::imwrite(ss.str(), frame);
            std::cout << ss.str() << std::endl;
            i += 1;
        }
    }
    
    cv::destroyAllWindows();
    return;
}