#include <tuple>
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "display.h"
#include "./../main.h"
#include "camera.h"

bool disp_on_Fove = true;
static double ex[2] = {-3.0, 3.0}, ey = 0.0, ez = 0.0, cx[2] = {-3.0, 3.0}, cy = 0.0, cz = 0.0;
static double ii = 0.0;

// ディスプレイまでの距離をピクセルの値に合わせた、ディスプレイが5.8インチ、ディスプレイまでの距離が4.05cm、解像度2560×1440から計算した
double dx = EYE_DISP_X * 4.05 / 3.21, dy = EYE_DISP_Y * 4.05 / 3.21;

cv::Mat input_image;

double x=0;
double y=0;

double Display::passed;


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

Display::Display(int h, int w, int argc, char* argv[]):
    height(h),
    width(w)
    {
        cv::Mat image=cv::imread("./../display_test.jpeg", 1);
        resize(image, image, cv::Size(), DISP_WIDTH/image.cols, DISP_HEIGHT/image.rows);
        cv::cvtColor(image, input_image, cv::COLOR_BGR2RGB);
        
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA);
        glutInitWindowPosition(0, 0);
        
        glutCreateWindow("fove display");
        glutFullScreen();
        glClearColor(0.5, 0.5, 0.5, 0.0); // init
        glutDisplayFunc(my_display_func);
        glutMainLoop();
    }

Display::~Display(void){}

void Display::my_display_func(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3d(1.0, 0.0, 1.0);
    // 左目
    glViewport(0, 0, DISP_WIDTH/2, DISP_HEIGHT);
    display_for_one_eye(0);
    //glColor3d(1.0, 0.0, 1.0);
    // shiten(screen_left[temp_num].x*dx, screen_left[temp_num].y*dy);  // 左目の視点表示 *視点表示したくない場合はこの行をコメントアウト

    // 右目
    glViewport(DISP_WIDTH/2, 0, DISP_WIDTH/2, DISP_HEIGHT);
    display_for_one_eye(1);
    //glColor3d(0.0, 1.0, 1.0);
    // shiten(screen_right[temp_num].x*dx, screen_right[temp_num].y*dy);  // 右目の視点表示 *視点表示したくない場合はこの行をコメントアウト
    glutPostRedisplay();
    glutSwapBuffers();
}

void Display::display_for_one_eye(int i){ // 片方のディスプレイ（i=0が左,i=1が右
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

    // 2D描画
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-EYE_DISP_X, EYE_DISP_X, -EYE_DISP_Y, EYE_DISP_Y, -1.0, 1.0);

    // 画像と図形の表示
    //show_image(input_image);
    //show_polygon();
    passed = get_passed_time();
    if(passed <= 30.0){
        calibration();
    }
}

void Display::show_image(cv::Mat image){
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.size().width, image.size().height, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glColor3d(1.0, 1.0, 1.0);
    put_2d_image_cv_ishikawa(0, 0, image.size().width, image.size().height, 1.0);
}

void Display::show_polygon(void){    
    glBegin(GL_POLYGON);
    glVertex2d(x-CALIB_SQURE, y-CALIB_SQURE);
    glVertex2d(x-CALIB_SQURE, y+CALIB_SQURE);
    glVertex2d(x+CALIB_SQURE, y+CALIB_SQURE);
    glVertex2d(x+CALIB_SQURE, y-CALIB_SQURE);
    glEnd();
}

void Display::calibration(void){
    if(3.0 < passed && passed <= 8.0){
        show_polygon();
    }else if(8.0 < passed && passed <= 10.0){
        y = (passed-8.0)*100; // 0->200
        show_polygon();
    }else if(10.0 < passed && passed <= 15.0){
        x = 0.0;
        y = 200.0;
        show_polygon();
    }else if(15.0 < passed && passed <= 17.0){
        x = (passed-15.0)*100; // 0->200
        y = 200-(passed-15.0)*100; // 200->0
        show_polygon();
    }else if(17.0 < passed && passed <= 22.0){
        x = 200.0;
        y = 0.0;
        show_polygon();
    }else if(22.0 < passed && passed <= 24.0){
        x = 200 - (passed-22.0)*100; // 200->0
        y = 0 - (passed-22.0)*100; // 0->-200
        show_polygon();
    }else if(24.0 < passed && passed <= 29.0){
        x = 0.0;
        y = -200.0;
        show_polygon();
    }else if(29.0 < passed && passed <= 31.0){
        x = 0 - (passed-29.0)*100; // 0->-200
        y = -200 + (passed-29.0)*100; // -200->0
        show_polygon();
    }else if(31.0 < passed && passed <= 36.0){
        x = -200.0;
        y = 0.0;
        show_polygon();
    }
}