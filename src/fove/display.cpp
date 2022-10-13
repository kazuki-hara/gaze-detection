#include <tuple>
#include <iostream>
#include <math.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "display.h"
#include "./../main.h"
#include "./../camera/camera.h"
#include "./opengl.h"

#define logitech "Logitech StreamCam"

bool disp_on_Fove = true;
static double ex[2] = {-3.0, 3.0}, ey = 0.0, ez = 0.0, cx[2] = {-3.25, 3.25}, cy = 0.0, cz = 0.0;
static double ii = 0.0;

// ディスプレイまでの距離をピクセルの値に合わせた、ディスプレイが5.8インチ、ディスプレイまでの距離が4.05cm、解像度2560×1440から計算した
double dx = EYE_DISP_X * 4.05 / 3.21, dy = EYE_DISP_Y * 4.05 / 3.21;


Display::Display(int h, int w):
    height(h),
    width(w)
    {
        frame_num = 0;
    }

Display::~Display();

Display::show_graphic(int argc, char* argv[]){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    
    glutCreateWindow("fove display");
    glutFullScreen();
    glClearColor(0.5, 0.5, 0.5, 0.0); // init
    glutDisplayFunc(my_display_func);
    glutMainLoop();
}