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
#include "./fove/camera.h"
#include "./gaze/gaze.h"
#include "./fove/display.h"
#include "main.h"


std::chrono::system_clock::time_point  start, now;
double msec;

double lx=0,ly=0,rx=0,ry=0;


bool console_print = true;


// データ出力先
std::string output_dir = "./test/";
FILE* test;
FILE* gaze_row_output;
FILE* gaze_output;

// foveのカメラから目の画像を取得して色々画像処理するスレッド (OpenCV)
void fove_camera(void){
    EyeCamera eye_camera(640, 280, "Fove Eyes: Fove Eyes");
    eye_camera.capture();
}

// foveのディスプレイに色々映すスレッド (OpenGL) 現状はキャリブレーション用の点のみ
void fove_display(int argc, char* argv[]){
    Display display(DISP_HEIGHT, DISP_WIDTH, argc, argv);
}

// 時間管理用スレッド
void record_passed_time(void){
    while(true){
        now = std::chrono::system_clock::now();
        auto passed = now-start;
        msec = (double)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000;
    }
}

// パラメータ管理のスレッド 現状は視線方向のみ管理　
void record_params(void){
    double prev_time = get_passed_time();
    while(true){
        double this_time = get_passed_time();
        if(this_time - prev_time > 0.001){
            std::tuple<double, double, double, double> gaze_info = get_gaze_info();
            lx = std::get<0>(gaze_info);
            ly = std::get<1>(gaze_info);
            rx = std::get<2>(gaze_info);
            ry = std::get<3>(gaze_info);
            if(lx != -1 && ly != -1 && rx != -1 && ry != -1 && console_print){
                std::cout << lx << " " << ly << " " << rx << " " << ry << " " << get_passed_time() << std::endl;
                fprintf(gaze_output, "%f %f %f %f %f\n", lx, ly, rx, ry, get_passed_time());
            }
            fprintf(gaze_row_output, "%f %f %f %f %f\n", lx, ly, rx, ry, get_passed_time());
            prev_time = this_time;
        }
    }
}

// プログラム開始から何秒経過したか取得する関数 (#include "main.h"で他ファイルでも使用可能)
double get_passed_time(void){
    return msec;
}

int main(int argc, char *argv[]){
    gaze_row_output = fopen("/share/home/hara/workspace/fove/data/gaze/gaze_row_data.txt", "w");
    gaze_output = fopen("/share/home/hara/workspace/fove/data/gaze/gaze.txt", "w");

    start = std::chrono::system_clock::now();
    std::thread fove_camera_thread(fove_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    std::thread time_thread(record_passed_time);
    std::thread params_thread(record_params);
    
    fove_camera_thread.detach();
    fove_display_thread.detach();
    time_thread.detach();
    params_thread.detach();
    
    while(true){
        if(get_passed_time() > 40){
            fclose(gaze_output);
            fclose(gaze_row_output);
            return 0;
        }
    }
    return 0;
}