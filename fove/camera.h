#include <opencv2/opencv.hpp>

class EyeCamera{
    public:
        EyeCamera(const int, const int, const char*);
        ~EyeCamera(void);

        void save_image(void);
        void capture(void);

        const int height;
        const int width;
        const char* dev_name;

    private:
        cv::Mat frame;

};