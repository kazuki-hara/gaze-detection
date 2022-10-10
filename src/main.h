#include <time.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#define LOG_RATE 10
double get_passed_time(void);
int check_mode(void);
std::tuple<double, double, double, double>get_disp_gaze(void);
bool check_detect_pupil_flag(void);
cv::Mat get_stereo_camera_frame(int);
cv::Mat get_obs_camera_frame(void);

class RemoteCameraControl{
    public:
        double get_passed_time(void); // 経過時間の取得
        int check_mode(void); // modeを取得
        std::tuple<double, double, double, double>get_disp_gaze(void); // 画面上の注視点を取得
        bool check_detect_pupil_flag(void);
        cv::Mat get_stereo_camera_frame(int);

    private:
        std::chrono::system_clock::time_point start, now; // プログラムの開始時刻、現在時刻
        bool exit_flag = false; // 処理の終了フラグ
        int mode; // 0->キャリブレーション　1->本番
        bool detect_pupil_flag = true; // 瞳孔検出フラグ
        std::tuple<double, double, double, double> disp_gaze; // 画面上の注視点（ピクセル座標) lx, ly, rx, ry
        cv::Mat stereo_camera_l_frame; // ステレオカメラ左画像
        cv::Mat stereo_camera_r_frame; // ステレオカメラ右画像
};