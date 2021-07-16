#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>


void camera_thread(void);



int main(void){
    std::thread cam_thread(camera_thread);
    cam_thread.join();
    std::cout << "hello" << std::endl;
    return 0;
}