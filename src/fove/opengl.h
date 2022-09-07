#ifndef OPENGL_H
#define OPENGL_H
#include <GL/glut.h>
#include <vector>

class OpenGL
{
public:
    void init(float r, float g, float b, float a);
    void init2(float r, float g, float b, float a);
	void DrawLine_2d(GLfloat width, GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
	void DrawLine(GLfloat width, GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2);
    void xy_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble angle_x, GLdouble angle_y, GLdouble angle_z);
    void yz_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height);
    void zx_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height);
    void xy_circle_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble r);
    void xy_circle(GLdouble x, GLdouble y, GLdouble z, GLdouble r, GLdouble angle_x, GLdouble angle_y, GLdouble angle_z);
    void circle_fill(GLdouble x, GLdouble y, GLdouble r);
    void square_fill(GLdouble x, GLdouble y, GLdouble width, GLdouble height);
    void DrawString(double x, double y, std::string s);
    void put_xy_image(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble rotation, GLdouble div);
    void put_xy_image_cv(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble rotation, GLdouble div);
    void redraw(int window1, int window2);
};
#endif


#define DATA_SIZE 100
#define FOVE_DISPLAY_WIDTH 640.0
#define FOVE_DISPLAY_HEIGHT 720.0
#define FOVE_DISTANCE_TO_DISPLAY_CENTIMETER 4.05
#define FOVE_DISPLAY_WIDTH_CENTIMETER 3.21
#define FOVE_DISTANCE FOVE_DISPLAY_WIDTH * FOVE_DISTANCE_TO_DISPLAY_CENTIMETER / FOVE_DISPLAY_WIDTH_CENTIMETER
#define EYE_DISTANCE 6.5

class Gaze
{
private:
	struct buffer
	{
		const void* data;
		size_t length;
	};
public:
    struct xy
    {
        float x;
        float y;
    };
    struct xyz
    {
        float x;
        float y;
        float z;
    };
	Gaze();
    int data_flag;
	float time;
	int number;
	int frameId;
	int timestamp;
	bool isCalibrating;
	bool isCalibrated;
	float convergenceRayDistance;
	float pupilDilation[DATA_SIZE];
	bool attention;
    int displaydataflag;
	std::vector<xyz> left_data, right_data;
    std::vector<xy> left_angle, right_angle;
    std::vector<xy> left_screen, right_screen;
    xyz e[2], c[2];
	buffer limage[DATA_SIZE], rimage[DATA_SIZE];
    xy headangle;
};