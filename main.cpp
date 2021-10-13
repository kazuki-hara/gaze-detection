#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>
#include <time.h>
#include <chrono>
#include <tuple>
#include "fove/camera.h"
#include "gaze/gaze.h"
#include "fove/display.h"
#include "main.h"

std::chrono::system_clock::time_point  start, now;
//auto msec;
double msec;


void fove_camera(){
    EyeCamera eye_camera(640, 280, "Fove Eyes: Fove Eyes");
    eye_camera.capture();
}

void fove_display(int argc, char* argv[]){
    Display display(DISP_HEIGHT, DISP_WIDTH, argc, argv);
}

void record_passed_time(){
    while(true){
        now = std::chrono::system_clock::now();
        auto passed = now-start;
        msec = (double)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000;
    }
}

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
        if(get_passed_time() > 40) return 0;
    }
    return 0;
}