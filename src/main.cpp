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




bool console_print = true;


// データ出力先
std::string output_dir = "./test/";
FILE* test;


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

// プログラム開始から何秒経過したか取得する関数 (#include "main.h"で他ファイルでも使用可能)
double get_passed_time(void){
    return msec;
}

int main(int argc, char *argv[]){
    start = std::chrono::system_clock::now();
    std::thread fove_camera_thread(fove_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    std::thread time_thread(record_passed_time);
    
    fove_camera_thread.detach();
    fove_display_thread.detach();
    time_thread.detach();
    
    while(true){
        if(get_passed_time() > 40){
            return 0;
        }
    }
    return 0;
}