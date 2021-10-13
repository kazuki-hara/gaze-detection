#include <sstream>
#include <opencv2/opencv.hpp>

#include "gaze.h"

#define image_dir "./../../data/image/"


int main(void){
    for(int i = 1; i<=1; i++){
        std::stringstream input;
        input << image_dir << i << ".png";
        cv::Mat original_image = cv::imread(input.str());

        EyeInfoGetter eyeinfo;
        std::tuple<cv::Mat, cv::Mat> images = eyeinfo.convert_image(original_image);
        cv::Mat left = std::get<0>(images);
        cv::Mat right = std::get<1>(images);
        eyeinfo.preprocess(left);
        eyeinfo.preprocess(right);
        std::vector<cv::Vec3f> circles = eyeinfo.make_hough_circle(left);
        cv::Mat circled_image = eyeinfo.draw_circles(left, circles);
        std::stringstream output;
        output << "./image/" << i << ".png";
        cv::imwrite("left.png", left);
        cv::imwrite("right.png", right);
        cv::imwrite("circle.png", circled_image);
    }
    return 0;
}