#include <opencv2/opencv.hpp>
#include <tuple>

class EyeCamera{
    public:
        EyeCamera();
        ~EyeCamera(void);

        void save_image(void);
        void capture(void);

        static double passed;

    private:
        cv::Mat frame;

};

std::tuple<double, double, double, double> get_gaze_info(void);