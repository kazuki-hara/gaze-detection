#include <opencv2/opencv.hpp>
#include <tuple>

class EyeCamera{
    public:
        EyeCamera(const int, const int, const char*);
        ~EyeCamera(void);

        void save_image(void);
        void capture(void);

        const int height;
        const int width;
        const char* dev_name;
        static double passed;

    private:
        cv::Mat frame;

};

std::tuple<double, double, double, double> get_gaze_info(void);