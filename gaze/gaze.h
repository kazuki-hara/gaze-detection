#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <vector>

#define BINARY_THRES 50

#define FIRST_GAUSS_K_SIZE 11
#define SECOND_GAUSS_K_SIZE 3
#define GAUSS_X_SIGMA 0
#define GAUSS_Y_SIGMA 0

#define CANNY_THRES 80

#define HOUGH_VOTE 20
#define HOUGH_MIN_CENTER_DIST 50
#define HOUGH_MIN_RADIUS 3
#define HOUGH_MAX_RADIUS 20

class EyeInfoGetter{
public:
    EyeInfoGetter();
    ~EyeInfoGetter();

    std::tuple<cv::Mat, cv::Mat> convert_image(cv::Mat);
    void preprocess(cv::Mat&);
    std::vector<cv::Vec3f>make_hough_circle(cv::Mat);
    cv::Mat draw_circles(cv::Mat, std::vector<cv::Vec3f>);

private:
};