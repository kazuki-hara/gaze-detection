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


#define Fove "FOVE Eyes: FOVE Eyes"
#define PayCam "PayCam: PayCam"

std::chrono::system_clock::time_point  start, now;
double msec;
bool exit_flag = false;
int mode;
std::tuple<double, double, double, double> disp_gaze;



//Camera stereo_camera(PayCam);
Display display(DISP_HEIGHT, DISP_WIDTH);

// データ出力先
std::string output_dir = "./test/";
FILE* test;

// プログラム開始から何秒経過したか取得する関数 (#include "main.h"で他ファイルでも使用可能)
double get_passed_time(void){
    return msec;
}

// ステレオカメラの画像を取得するスレッド (OpenCV)
//void cammount_camera(void){
//    stereo_camera.capture();
//}

// foveのカメラから目の画像を取得するスレッド (OpenCV)
void fove_camera(Camera* eye_camera){
    eye_camera->capture();
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

std::tuple<double, double, double, double>get_disp_gaze(void){
    return disp_gaze;
}

int main(int argc, char *argv[]){
    std::cout << argv[1] << std::endl;
    if(argv[1] == "calibration") mode = 0;
    else mode = 1;


    Camera eye_camera(Fove);
    start = std::chrono::system_clock::now();
    std::thread fove_camera_thread(fove_camera, &eye_camera);
    //std::thread cammount_camera_thread(cammount_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    std::thread time_thread(record_passed_time);
    
    //cammount_camera_thread.detach();
    fove_camera_thread.detach();
    fove_display_thread.detach();
    time_thread.detach();
    
    EyeInfoGetterV2 eye_info_getter;

    FILE* time_log;
    FILE* pupil_log;
    FILE* gaze_log;
    if(mode == 0){
        time_log = fopen("/share/home/hara/Data/fove/tmp/time0.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil0.txt", "w");
    }else{
        time_log = fopen("/share/home/hara/Data/fove/tmp/time1.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil1.txt", "w");
        gaze_log = fopen("/share/home/hara/Data/fove/tmp/gaze1.txt", "w");
    }
    
    std::string image_dir = "/share/home/hara/Data/fove/tmp/image/";

    int i = 0;

    while(true){
        if(eye_camera.check_device_opened()){
            cv::Mat frame = eye_camera.get_frame();
            fprintf(time_log, "%f\n", get_passed_time());
            //cv::imwrite(image_dir + std::to_string(i) + ".png", frame);
            std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(frame);
            double lx = std::get<0>(pupil_pos);
            double ly = std::get<1>(pupil_pos);
            double rx = std::get<2>(pupil_pos);
            double ry = std::get<3>(pupil_pos);
            fprintf(pupil_log, "%f %f %f %f\n", lx, ly, rx, ry);

            if(mode == 1){
                disp_gaze = calib.calcuration_gaze_point(lx, ly, rx, ry);
            }

            //cv::Mat pupil_frame = eye_info_getter.draw_pupil_center(frame, pupil_pos);
            //cv::imshow("Fove", pupil_frame);
        }
        //if(stereo_camera.check_device_opened()){
        //    cv::Mat s_frame = stereo_camera.get_frame();
        //    cv::imshow("雲台", s_frame);
        //}
        if(mode == 0){
            if(get_passed_time() > 175){
                std::cout << "finish" << std::endl;
                fclose(time_log);
                fclose(gaze_log);
                break;
            }
        }
        
        i++;
    }
    return 0;
}