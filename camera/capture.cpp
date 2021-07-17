/* 
雲台に搭載されたカメラから画像を取得
*/

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

cv::Mat frame;
cv::Mat l_frame;
cv::Mat r_frame;
double ratio = 0.5;

void camera_thread(void){
    cv::VideoCapture cap(0);
    cv::namedWindow("video");
    while(true){
        cap >> frame;
        if(frame.empty()) break;
        cv::Rect left(0, 0, 1280, 960);
		cv::Rect right(1280, 0, 1280, 960);
		cv::Mat left_frame(frame, left);
		cv::Mat right_frame(frame, right);
		cv::cvtColor(left_frame, l_frame, cv::COLOR_BGR2RGB);
		cv::cvtColor(right_frame, r_frame, cv::COLOR_BGR2RGB);
        cv::resize(left_frame, left_frame, cv::Size(), ratio, ratio);
        cv::imshow("video", left_frame);
        if(cv::waitKey(1) >= 0) break;
    }
}

