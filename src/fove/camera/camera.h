#include <opencv2/opencv.hpp>
#include <tuple>
#include <string>

class EyeCamera{
    public:
        EyeCamera(std::string);
        ~EyeCamera(void);

        void save_image(void);
        void capture(void);
        cv::Mat get_frame(void);

        static double passed;

    private:
        cv::Mat frame;
        int dev_id;

};

std::tuple<double, double, double, double> get_gaze_info(void);