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
bool detect_pupil_flag = false;
bool gaze_in_disp_flag = false;
std::tuple<double, double, double, double> disp_gaze;
cv::Mat stereo_camera_l_frame;
cv::Mat stereo_camera_r_frame;

cv::Mat obs_camera_frame;

//Camera stereo_camera(PayCam);
Display display(DISP_HEIGHT, DISP_WIDTH);

// データ出力先
std::string output_dir = "./test/";
FILE* test;

// スレッド終了判定フラグ
bool camera_thread_exit_flag = false;
bool cammount_thread_exit_flag = false;
bool display_thread_exit_flag = false;

// プログラム開始から何秒経過したか取得する関数 (#include "main.h"で他ファイルでも使用可能)
double get_passed_time(void){
    return msec;
}

void cammount_serial(void){
    if(mode == 1){
        Cammount cammount("cammount", 4000);
        if(cammount.check_connection()){
            cammount.connection();
            cammount.send_command();
        };
    }
    cammount_thread_exit_flag = true;
    return;
}

// ステレオカメラの画像を取得するスレッド (OpenCV)
void stereo_camera_thread_func(Camera* stereo_camera){
    stereo_camera->capture();
    return;
}

// foveのカメラから目の画像を取得するスレッド (OpenCV)
void fove_camera(Camera* eye_camera){
    eye_camera->capture();
    camera_thread_exit_flag = true;
    return;
}


void obs_camera_func(Camera* obs_camera){
    obs_camera->capture();
    return;
}

// foveのディスプレイに色々映すスレッド (OpenGL) 現状はキャリブレーション用の点のみ
void fove_display(int argc, char* argv[]){
    display.show_graphic(argc, argv);
    display_thread_exit_flag = true;
    return;
}


// 時間管理用スレッド
void record_passed_time(void){
    while(true){
        now = std::chrono::system_clock::now();
        auto passed = now-start;
        msec = (double)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000;
        if(exit_flag) break;
    }
    return;
}

int check_mode(void){
    return mode;
}

bool check_exit_flag(void){
    return exit_flag;
}

bool check_detect_pupil_flag(void){
    return detect_pupil_flag;
}

bool check_gaze_in_disp_flag(void){
    return gaze_in_disp_flag;
}

std::tuple<double, double, double, double>get_gaze_pixel(void){
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
    
    std::thread fove_camera_thread(fove_camera, &eye_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    std::thread cammount_thread(cammount_serial);
    std::thread time_thread(record_passed_time);
    
    fove_camera_thread.detach();
    fove_display_thread.detach();
    time_thread.detach();
    if(mode == 1) cammount_thread.detach();
    
    EyeInfoGetterV2 eye_info_getter;

    FILE* time_log;
    FILE* pupil_log;
    FILE* gaze_log;

    
    if(mode == 0){
        time_log = fopen("/share/home/hara/Data/fove/tmp/time0.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil0.txt", "w");
        //time_log = fopen("/share/home/hara/Data/fove/pupil/saito/300/time0.txt", "w");
        //pupil_log = fopen("/share/home/hara/Data/fove/pupil/saito/300/pupil0.txt", "w");
    }else{
        time_log = fopen("/share/home/hara/Data/fove/tmp/time1.txt", "w");
        pupil_log = fopen("/share/home/hara/Data/fove/tmp/pupil1.txt", "w");
        gaze_log = fopen("/share/home/hara/Data/fove/tmp/gaze1.txt", "w");
    }
    
    std::string image_dir = "/share/home/hara/Data/fove/20221025/hara/image/";

    int i = 0;
    
    Calibration calib("/share/home/hara/workspace/fove/config/calibration/parameter.txt");

    start = std::chrono::system_clock::now();

    while(true){
        if(eye_camera.check_device_opened()){
            cv::Mat frame = eye_camera.get_frame();
            
            //cv::imwrite(image_dir + std::to_string(i) + ".png", frame);
            std::tuple<double, double, double, double> pupil_pos = eye_info_getter.detect_pupil_center(frame);
            double lx = std::get<0>(pupil_pos);
            double ly = std::get<1>(pupil_pos);
            double rx = std::get<2>(pupil_pos);
            double ry = std::get<3>(pupil_pos);
            fprintf(time_log, "%f\n", get_passed_time());
            fprintf(pupil_log, "%f %f %f %f\n", lx, ly, rx, ry);

            detect_pupil_flag = eye_info_getter.check_detect_pupil();

            if(mode == 1 && detect_pupil_flag == true){
                disp_gaze = calib.calcuration_gaze_point(lx, ly, rx, ry);
                fprintf(gaze_log, "%f %f %f %f\n", std::get<0>(disp_gaze), std::get<1>(disp_gaze), std::get<2>(disp_gaze), std::get<3>(disp_gaze));
                gaze_in_disp_flag = calib.check_gaze_in_disp_flag();
            }else if(mode == 1 && detect_pupil_flag == false){
                fprintf(gaze_log, "%d %d %d %d\n", -1, -1, -1, -1);
                gaze_in_disp_flag = false;
            }
            i++;
        }

        if(mode == 0){
            if(get_passed_time() > 125){
                std::cout << "finish" << std::endl;
                fclose(time_log);
                fclose(pupil_log);
                exit_flag = true;
                break;
            }
        }
        if(mode == 1){
            if (kbhit()){
                if(getchar() == 'q'){
                    fclose(time_log);
                    fclose(gaze_log);
                    fclose(pupil_log);
                    exit_flag = true;
                    break;
                }
            }
        }
    }
    while(true){
        if(display_thread_exit_flag == true && cammount_thread_exit_flag == true && camera_thread_exit_flag == true) break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}