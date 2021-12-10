#include <GL/glut.h>


#define DISP_WIDTH  2560
#define DISP_HEIGHT 1440
#define EYE_DISP_X 640
#define EYE_DISP_Y 720

#define CALIB_SQURE 10


class Display{
public:
    Display(int, int);
    ~Display();

    void show_graphic(int, char*[]);

    void show_cammount_video(void);
    void display_calibration_scene(void);
    static void calibration(void);
    
    const int height;
    const int width; 

private:
    static void my_display_func(void);
    static void display_for_one_eye(int);
    static void show_image(cv::Mat);
    static void show_polygon(void);
    static void show_calib_poinst(int, int);
    static double passed;
};

void put_2d_image_cv_ishikawa(GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLdouble div);