#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>

#include "./fove/camera.h"



int main(void){
    EyeCamera eye_camera(640, 280, "Fove Eyes: Fove Eyes");
    eye_camera.capture();
    return 0;
}