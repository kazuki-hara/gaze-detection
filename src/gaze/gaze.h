#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <tuple>
#include <vector>

#define BINARY_THRES 30

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


class EyeInfoGetterV2{
public:
    EyeInfoGetterV2();
    ~EyeInfoGetterV2();

    std::tuple<cv::Mat, cv::Mat> convert_image(cv::Mat);
    void preprocess(cv::Mat&);
    std::vector<cv::Vec3f> detect_pupil_circle(cv::Mat);
    int select_pupil_circle(cv::Mat, std::vector<cv::Vec3f>);
    std::tuple<double, double> cal_center_of_gravity(cv::Mat, std::vector<cv::Vec3f>);
    std::tuple<double, double> cal_center_of_gravity_v2(cv::Mat, cv::Vec3f);
    std::tuple<double, double, double, double> detect_pupil_center(cv::Mat);
    std::tuple<double, double, double, double> detect_pupil_center_v2(cv::Mat);
    std::tuple<bool, cv::RotatedRect> pupil_center_ellipse(cv::Mat, cv::Vec3f);

    cv::Mat draw_pupil_center(cv::Mat, std::tuple<double, double, double, double>);
    std::tuple<cv::RotatedRect, cv::RotatedRect> get_pupil_info(void);

private:
    double l_x, l_y, r_x, r_y;
    cv::RotatedRect left_box, right_box;
    cv::Mat blur_image;
};

//void get_pupil_center(double, double, cv::Mat);
void remove_blank_line(cv::Mat &);

cv::Mat draw_hough_circles(cv::Mat, std::vector<cv::Vec3f>, std::vector<cv::Vec3f>);
cv::Mat draw_hough_circle(cv::Mat, cv::Vec3f);
cv::Mat draw_pupil_edges(cv::Mat, cv::Mat, std::vector<cv::Vec3f>);
cv::Mat draw_pupil_edge(cv::Mat, int, int);

cv::Mat remove_outside_area_v3(cv::Mat);