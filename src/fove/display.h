#include <GL/glut.h>
#include <tuple>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#define DISP_WIDTH  2560
#define DISP_HEIGHT 1440
#define EYE_DISP_X 640
#define EYE_DISP_Y 720

#define CALIB_SQURE 10


class Display{
public:
    Display(int, int);
    ~Display(void);

    void show_graphic(int, char*[]);
    
    const int height;
    const int width;
    

};

void my_display_func(void);