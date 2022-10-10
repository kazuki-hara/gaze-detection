#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>
#include <time.h>
#include <chrono>
#include <tuple>
#include "./camera/camera.h"
#include "./gaze/gaze.h"
#include "./fove/display.h"
#include "main.h"
#include "./utils/utils.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <unistd.h>
#include "./calibration/calibration.h"
#include "./cammount/cammount.h"


#define Fove "FOVE Eyes: FOVE Eyes"
#define PayCam "PayCam: PayCam"
#define OBSBOT "Logitech StreamCam"

std::chrono::system_clock::time_point start, now;
double msec;
bool exit_flag = false;
int mode;
bool detect_pupil_flag = true;
std::tuple<double, double, double, double> disp_gaze;
cv::Mat stereo_camera_l_frame;
cv::Mat stereo_camera_r_frame;

cv::Mat obs_camera_frame;

//Camera stereo_camera(PayCam);
Display display(DISP_HEIGHT, DISP_WIDTH);

// データ出力先
std::string output_dir = "./test/";
FILE* test;

// プログラム開始から何秒経過したか取得する関数 (#include "main.h"で他ファイルでも使用可能)
double get_passed_time(void){
    return msec;
}

void cammount_serial(void){
    Cammount cammount("cammount", 4000);
    if(cammount.check_connection()){
        std::cout << "check_connection() -> true" << std::endl;
        cammount.connection();
        std::cout << "connected" << std::endl;
        cammount.send_command();
    };
}

// ステレオカメラの画像を取得するスレッド (OpenCV)
void stereo_camera_thread_func(Camera* stereo_camera){
    stereo_camera->capture();
}

// foveのカメラから目の画像を取得するスレッド (OpenCV)
void fove_camera(Camera* eye_camera){
    eye_camera->capture();
}


void obs_camera_func(Camera* obs_camera){
    obs_camera->capture();
}

// foveのディスプレイに色々映すスレッド (OpenGL) 現状はキャリブレーション用の点のみ
void fove_display(int argc, char* argv[]){
    display.show_graphic(argc, argv);
}


// 時間管理用スレッド
void record_passed_time(void){
    while(true){
        now = std::chrono::system_clock::now();
        auto passed = now-start;
        msec = (double)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000;
    }
}

int check_mode(void){
    return mode;
}

bool check_detect_pupil_flag(void){
    return detect_pupil_flag;
}

std::tuple<double, double, double, double>get_disp_gaze(void){
    return disp_gaze;
}

cv::Mat get_stereo_camera_frame(int i){
    if(i==0){
        return stereo_camera_l_frame;
    }
    else return stereo_camera_r_frame;
}

cv::Mat get_obs_camera_frame(void){
    return obs_camera_frame;
}

int main(int argc, char *argv[]){
    mode = std::stoi(argv[1]);
    std::cout << mode << std::endl;

    Camera eye_camera(Fove);
    //Camera stereo_camera(PayCam);
    

    start = std::chrono::system_clock::now();
    std::thread fove_camera_thread(fove_camera, &eye_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    //std::thread stereo_camera_thread(stereo_camera_thread_func, &stereo_camera);
    //std::thread obs_camera_thread(obs_camera_func, &obs_camera);
    std::thread cammount_thread(cammount_serial);
    std::thread time_thread(record_passed_time);
    
    //cammount_camera_thread.detach();
    fove_camera_thread.detach();
    fove_display_thread.detach();
    //stereo_camera_thread.detach();
    //obs_camera_thread.detach();
    time_thread.detach();
    cammount_thread.detach();
    
    EyeInfoGetterV2 eye_info_getter;

    FILE* time_log;
    FILE* pupil_log;
    FILE* gaze_log;

    
    if(mode == 0){
        //time_log = fopen("/share/home/hara/Data/fove/tmp/time0.txt", "w");
        //pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil0.txt", "w");
        time_log = fopen("/share/home/hara/Data/fove/pupil/saito/300/time0.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/pupil/saito/300/pupil0.txt", "w");
    }else{
        time_log = fopen("/share/home/hara/Data/fove/tmp/time1.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil1.txt", "w");
        gaze_log = fopen("/share/home/hara/Data/fove/tmp/gaze1.txt", "w");
    }
    
    std::string image_dir = "/share/home/hara/Data/fove/pupil/saito/300/image/";

    int i = 0;
    std::chrono::system_clock::time_point frame_1, frame_101;
    
    Calibration calib("/share/home/hara/workspace/fove/config/calibration/parameter.txt");

    while(true){
        if(i == 1) frame_1 = std::chrono::system_clock::now();
        if(i == 101){
            frame_101 = std::chrono::system_clock::now();
            double main_fps = (double)std::chrono::duration_cast<std::chrono::milliseconds>(frame_101 - frame_1).count() / 1000;
            std::cout << "main fps: " << main_fps << std::endl;
        }
        if(eye_camera.check_device_opened()){
            cv::Mat frame = eye_camera.get_frame();
            fprintf(time_log, "%f\n", get_passed_time());
            cv::imwrite(image_dir + std::to_string(i) + ".png", frame);
            std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(frame);
            double lx = std::get<0>(pupil_pos);
            double ly = std::get<1>(pupil_pos);
            double rx = std::get<2>(pupil_pos);
            double ry = std::get<3>(pupil_pos);
            fprintf(pupil_log, "%f %f %f %f\n", lx, ly, rx, ry);

            if(lx != -1 || ly != -1 || rx != -1 || ry != -1) detect_pupil_flag = true;
            else detect_pupil_flag = false;

            if(mode == 1){
                disp_gaze = calib.calcuration_gaze_point(lx, ly, rx, ry);
            }

            //cv::Mat pupil_frame = eye_info_getter.draw_pupil_center(frame, pupil_pos);
            //cv::imshow("Fove", pupil_frame);
        }
        /*if(stereo_camera.check_device_opened()){
            cv::Mat stereo_camera_frame = stereo_camera.get_frame();
            cv::Rect left(0, 0, 1280, 960);
		    cv::Rect right(1280, 0, 1280, 960);
		    stereo_camera_l_frame = cv::Mat(stereo_camera_frame, left);
		    stereo_camera_r_frame = cv::Mat(stereo_camera_frame, right);
            //std::cout << stereo_camera_frame.size().height << " " << stereo_camera_frame.size().width << std::endl;
        }*/
        if(mode == 0){
            if(get_passed_time() > 125){
                std::cout << "finish" << std::endl;
                fclose(time_log);
                fclose(pupil_log);
                break;
            }
        }
    }
    
    return 0;
}