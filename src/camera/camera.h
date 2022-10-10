#include <opencv2/opencv.hpp>
#include <string>

class Camera{
    public:
        Camera(std::string);
        ~Camera(void);

        void save_image(void);
        void capture(void);
        bool check_device_opened(void);
        cv::Mat get_frame(void);
        cv::Mat capture_one_frame(void);
        int get_dev_id(void);


    private:
        cv::Mat frame;
        int dev_id;
        bool dev_open;

};