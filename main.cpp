#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>

#include "fove/camera.h"
#include "gaze/gaze.h"
#include "fove/display.h"

void fove_camera(void){
    EyeCamera eye_camera(640, 280, "Fove Eyes: Fove Eyes");
    eye_camera.capture();
}

void fove_display(int argc, char* argv[]){
    Display display(DISP_HEIGHT, DISP_WIDTH, argc, argv);
}

int main(int argc, char *argv[]){
    std::thread fove_camera_thread(fove_camera);
    std::thread fove_display_thread(fove_display, argc, argv);
    fove_camera_thread.join();
    fove_display_thread.join();
    return 0;
}