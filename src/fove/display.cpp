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
#define PayCam "PayCam: PayCam"

#define Calib

static double ex[2] = {-3.0, 3.0}, ey = 0.0, ez = 0.0, cx[2] = {-3.25, 3.25}, cy = 0.0, cz = 0.0;
static double ii = 0.0;

cv::VideoCapture cammount_camera_cap;
Camera cammount_camera(logitech);
cv::Mat cammount_camera_image_tmp;
cv::Mat cammount_camera_image;

double passed_time;
std::tuple<double, double, double, double> gaze_pixel;

void put_2d_image_cv_ishikawa(GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLdouble div)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, div);
    glVertex2d(x - width / 2.0, y - height / 2.0);
    glTexCoord2f(div, div);
    glVertex2d(x + width / 2.0, y - height / 2.0);
    glTexCoord2f(div, 0.0);
    glVertex2d(x + width / 2.0, y + height / 2.0);
    glTexCoord2f(0.0, 0.0);
    glVertex2d(x - width / 2.0, y + height / 2.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void show_image(cv::Mat image){
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.size().width, image.size().height, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    put_2d_image_cv_ishikawa(0, 0, image.size().width, image.size().height, 1.0);
}

void show_polygon(int x, int y, double red, double green, double blue){
    glBegin(GL_POLYGON);
    glColor3d(red, green, blue);
    glVertex2d(x-CALIB_SQURE/2, y-CALIB_SQURE/2);
    glVertex2d(x-CALIB_SQURE/2, y+CALIB_SQURE/2);
    glVertex2d(x+CALIB_SQURE/2, y+CALIB_SQURE/2);
    glVertex2d(x+CALIB_SQURE/2, y-CALIB_SQURE/2);
    glEnd();
}

void show_xy_axis(void){
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2d(0, 720);
    glVertex2d(0, -720);
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(-640, 0);
    glVertex2d(640, 0);
    glEnd();
}

void calibration_v2(void){
    double range = 200.0;
    int index = ((int)passed_time) / 5;
    int x_index = index % 5;
    int y_index = (int)(index / 5);
    int x = -1* range + x_index * range / 2;
    int y = range - y_index * range / 2;
    show_polygon(x, y, 0.0, 1.0, 0.0);
}

void show_gaze_pixel(void){
    double lx = std::get<0>(gaze_pixel);
    double ly = std::get<1>(gaze_pixel);
    double rx = std::get<2>(gaze_pixel);
    double ry = std::get<3>(gaze_pixel);

    glColor3d(1.0, 0.1, 0.1);
    glBegin(GL_POLYGON);
    glVertex2d(lx - CALIB_SQURE/2, ly - CALIB_SQURE/2);
    glVertex2d(lx - CALIB_SQURE/2, ly + CALIB_SQURE/2);
    glVertex2d(lx + CALIB_SQURE/2, ly + CALIB_SQURE/2);
    glVertex2d(lx + CALIB_SQURE/2, ly - CALIB_SQURE/2);
    glEnd();

    glColor3d(0.1, 0.1, 1.0);
    glBegin(GL_POLYGON);
    glVertex2d(rx - CALIB_SQURE/2, ry - CALIB_SQURE/2);
    glVertex2d(rx - CALIB_SQURE/2, ry + CALIB_SQURE/2);
    glVertex2d(rx + CALIB_SQURE/2, ry + CALIB_SQURE/2);
    glVertex2d(rx + CALIB_SQURE/2, ry - CALIB_SQURE/2);
    glEnd();
}

void display_for_one_eye(int i){ // 片方のディスプレイ（i=0が左,i=1が右
    //----OpenGL上の目と頭の位置設定(3Dのときのみ)-----
    if(i==0) ex[i] = (ex[0]+ex[1])/2.0 - 3.0*cos(M_PI*ii/180.0);
    else if(i==1) ex[i] = (ex[0]+ex[1])/2.0 + 3.0*cos(M_PI*ii/180.0);
    cz = ez - 1000*cos(M_PI*ii/180.0);
    cx[i] = ex[i] - 1000*sin(M_PI*ii/180.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(106.563963618037, 2560.0/1440.0/2.0, 1.0, 1000.0);
    gluLookAt(ex[i], ey, ez, cx[i], cy, cz, 0.0, 1.0, 0.0);

    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3D描画
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glColor4d(1.0, 1.0, 1.0,1.0);

    // 2D描画
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-EYE_DISP_X, EYE_DISP_X, -EYE_DISP_Y, EYE_DISP_Y, -1.0, 1.0);


    if(check_mode() == 0) calibration_v2();
    else{
        show_image(cammount_camera_image);
        if(check_detect_pupil_flag()) show_gaze_pixel();
    }
    show_xy_axis();
}

void my_display_func(void){
    passed_time = get_passed_time();
    
    if(check_mode() == 1){
        if(cammount_camera_cap.isOpened()) cammount_camera_cap.read(cammount_camera_image);
        cv::cvtColor(cammount_camera_image, cammount_camera_image, cv::COLOR_BGR2RGB);
        gaze_pixel = get_gaze_pixel();
    }

    glClear(GL_COLOR_BUFFER_BIT);
    // 左目
    glViewport(0, 0, DISP_WIDTH/2, DISP_HEIGHT);
    display_for_one_eye(0);

    // 右目
    glViewport(DISP_WIDTH/2, 0, DISP_WIDTH/2, DISP_HEIGHT);
    display_for_one_eye(1);

    glutPostRedisplay();
    glutSwapBuffers();
}


void init(void){
    glClearColor(0.5, 0.5, 0.5, 1.0); // init
    std::cout << "init!" << std::endl;
}

Display::Display(int h, int w):
    height(h),
    width(w)
    {}

Display::~Display(void){}

void Display::show_graphic(int argc, char* argv[]){
    if(check_mode() ==1){
        cammount_camera_cap.open(cammount_camera.get_dev_id());
        cammount_camera_cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
        cammount_camera_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
        cammount_camera_cap.set(cv::CAP_PROP_FPS, 30);

        if(cammount_camera_cap.isOpened()) std::cout << "cap success!" << std::endl;
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("fove display");
    glutFullScreen();
    init();
    glutDisplayFunc(my_display_func);
    glutMainLoop();
}