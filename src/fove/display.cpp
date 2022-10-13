#include <tuple>
#include <iostream>
#include <math.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "display.h"
#include "./../main.h"
#include "./../camera/camera.h"
#include "./opengl.h"
//#include <GL/glut.h>
//#include <GL/freeglut.h>

#define logitech "Logitech StreamCam"
#define PayCam "PayCam: PayCam"

bool disp_on_Fove = true;
static double ex[2] = {-3.0, 3.0}, ey = 0.0, ez = 0.0, cx[2] = {-3.25, 3.25}, cy = 0.0, cz = 0.0;
static double ii = 0.0;

// ディスプレイまでの距離をピクセルの値に合わせた、ディスプレイが5.8インチ、ディスプレイまでの距離が4.05cm、解像度2560×1440から計算した
double dx = EYE_DISP_X * 4.05 / 3.21, dy = EYE_DISP_Y * 4.05 / 3.21;

cv::VideoCapture cammount_camera_cap;
Camera cammount_camera(logitech);
cv::Mat cammount_camera_image;

void my_display_func(void){
    if(cammount_camera_cap.isOpened()) cammount_camera_cap.read(cammount_camera_image);
    cv::cvtColor(cammount_camera_image, cammount_camera_image, cv::COLOR_BGR2RGB);


    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(cammount_camera_image.cols, cammount_camera_image.rows, GL_RGB, GL_UNSIGNED_BYTE, cammount_camera_image.data); // 画像表示
    glutPostRedisplay(); // これがないと次の描画するきっかけができないので
    glFlush();
}

void init(void){
    glClearColor(0.5, 0.5, 0.5, 0.0); // init
}

Display::Display(int h, int w):
    height(h),
    width(w)
    {}

Display::~Display(void){}

void Display::show_graphic(int argc, char* argv[]){

    cammount_camera_cap.open(cammount_camera.get_dev_id());
    cammount_camera_cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cammount_camera_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cammount_camera_cap.set(cv::CAP_PROP_FPS, 30);

    if(cammount_camera_cap.isOpened()) std::cout << "cap success!" << std::endl;
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGBA);

    glutCreateWindow("fove display");
    glutFullScreen();
    glutDisplayFunc(my_display_func);
    init();
    glutMainLoop();
    my_display_func();
}