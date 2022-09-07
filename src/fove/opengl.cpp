#include <string>
#include <math.h>
#include "opengl.h"
#include <vector>

Gaze::Gaze()
{
    e[0].x = 0.0; e[1].x = 0.0;
    //e[0].x = -EYE_DISTANCE / 2.0; e[1].x = EYE_DISTANCE / 2.0;
    e[0].y =  0.0; e[1].y = 0.0;
    e[0].z =  0.0; e[1].z = 0.0;
    c[0].x = -EYE_DISTANCE / 2.0; c[1].x = EYE_DISTANCE / 2.0;
    c[0].y =  0.0; c[1].y = 0.0;
    c[0].z =  -1000.0; c[1].z = -1000.0;
    headangle = {0.0, 0.0};
    data_flag = 1;
    displaydataflag = 0;
}

void OpenGL::init(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void OpenGL::init2(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void OpenGL::DrawLine_2d(GLfloat width, GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    glPushMatrix();
    glLineWidth(width);
    glBegin(GL_LINES); 
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glEnd();
    glPopMatrix();
}

void OpenGL::DrawLine(GLfloat width, GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2)
{
    glPushMatrix();
    glLineWidth(width);
    glBegin(GL_LINES); 
    glVertex3d(x1, y1, z1);
    glVertex3d(x2, y2, z2);
    glEnd();
    glLineWidth(1.0);
    glPopMatrix();
}

void OpenGL::xy_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble angle_x, GLdouble angle_y, GLdouble angle_z)
{
    glPushMatrix();
    glTranslated((x),(y),(z));
    glRotated(angle_z, 0.0, 0.0, 1.0);
    glRotated(angle_y, 0.0, 1.0, 0.0);
    glRotated(angle_x, 1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    //glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(- width/2.0, - height/2.0, 0);
    glVertex3d(width/2.0, - height/2.0, 0);
    glVertex3d(width/2.0, height/2.0, 0);
    glVertex3d(- width/2.0, height/2.0, 0);    
    /*glVertex3d(x - width/2.0, y - height/2.0, z);
    glVertex3d(x + width/2.0, y - height/2.0, z);
    glVertex3d(x + width/2.0, y + height/2.0, z);
    glVertex3d(x - width/2.0, y + height/2.0, z);    */
    glEnd();
    glPopMatrix();
}

void OpenGL::yz_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height)
{
    glPushMatrix();
    glTranslated((x),(y),(z));
    glBegin(GL_QUADS);
    glNormal3d(1.0, 0.0, 0.0);
    glVertex3d(x, y - width/2.0, z - height/2.0); 
    glVertex3d(x, y + width/2.0, z - height/2.0); 
    glVertex3d(x, y + width/2.0, z + height/2.0);
    glVertex3d(x, y - width/2.0, z + height/2.0);
    glEnd();
    glPopMatrix();
}

void OpenGL::zx_square_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height)
{
    glPushMatrix();
    glTranslated((x),(y),(z));
    glBegin(GL_QUADS);
    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(x - height/2.0, y, z - width/2.0); 
    glVertex3d(x - height/2.0, y, z + width/2.0);
    glVertex3d(x + height/2.0, y, z + width/2.0);
    glVertex3d(x + height/2.0, y, z - width/2.0); 
    glEnd();
    glPopMatrix();
}

void OpenGL::xy_circle_fill(GLdouble x, GLdouble y, GLdouble z, GLdouble r)
{
    int i;
    GLdouble x_tmp, y_tmp;

    glPushMatrix();
    glTranslated((x),(y),(z));
    glBegin(GL_POLYGON);
    for(i = 0;i <= 360; i += 10){
        x_tmp = r*cos(M_PI*i/180.0);
        y_tmp = r*sin(M_PI*i/180.0);
        glVertex3d(x_tmp, y_tmp, 0);
    }
    glEnd();
    glPopMatrix();
}

void OpenGL::xy_circle(GLdouble x, GLdouble y, GLdouble z, GLdouble r, GLdouble angle_x, GLdouble angle_y, GLdouble angle_z)
{
    int i;
    GLdouble x_tmp, y_tmp;

    glPushMatrix();
    glTranslated((x),(y),(z));
    glRotated(angle_z, 0.0, 0.0, 1.0);
    glRotated(angle_y, 0.0, 1.0, 0.0);
    glRotated(angle_x, 1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for(i = 0;i <= 360; i += 10){
        x_tmp = r*cos(M_PI*i/180.0);
        y_tmp = r*sin(M_PI*i/180.0);
        glVertex3d(x_tmp, y_tmp, 0);
    }
    glEnd();
    glPopMatrix();
}

void OpenGL::circle_fill(GLdouble x, GLdouble y, GLdouble r)
{
    int i;
    GLdouble x_tmp, y_tmp;

    glPushMatrix();
    glTranslated((x),(y),(0));
    glBegin(GL_POLYGON);
    for(i = 0;i <= 360; i += 10){
        x_tmp = r*cos(M_PI*i/180.0);
        y_tmp = r*sin(M_PI*i/180.0);
        glVertex2d(x_tmp, y_tmp);
    }
    glEnd();
    glPopMatrix();
}

void OpenGL::square_fill(GLdouble x, GLdouble y, GLdouble width, GLdouble height)
{
    glPushMatrix();
    glTranslated((x),(y),(0));
    glBegin(GL_QUADS);
    glVertex2d(x - width/2.0, y - height/2.0);
    glVertex2d(x + width/2.0, y - height/2.0);
    glVertex2d(x + width/2.0, y + height/2.0);
    glVertex2d(x - width/2.0, y + height/2.0);    
    glEnd();
    glPopMatrix();
}

void OpenGL::DrawString(double x, double y, std::string s)
{
    glPushMatrix();
    glRasterPos2f(x, y);
    for(int i=0; i<s.size(); ++i)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
    glPopMatrix();
}

void OpenGL::put_xy_image(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble rotation, GLdouble div)
{
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glRotated((rotation),(0),(0),(1));
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, div);
    glVertex3d(x - width / 2.0, y + height / 2.0, z);
    glTexCoord2f(div, div);
    glVertex3d(x + width / 2.0, y + height / 2.0, z);
    glTexCoord2f(div, 0.0);
    glVertex3d(x + width / 2.0, y - height / 2.0, z);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(x - width / 2.0, y - height / 2.0, z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void OpenGL::put_xy_image_cv(GLdouble x, GLdouble y, GLdouble z, GLdouble width, GLdouble height, GLdouble rotation, GLdouble div)
{
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, div);
    glVertex3d(x - width / 2.0, y - height / 2.0, z);
    glTexCoord2f(div, div);
    glVertex3d(x + width / 2.0, y - height / 2.0, z);
    glTexCoord2f(div, 0.0);
    glVertex3d(x + width / 2.0, y + height / 2.0, z);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(x - width / 2.0, y + height / 2.0, z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void OpenGL::redraw(int window1, int window2)
{
    glutSetWindow(window1);
    glutPostRedisplay();
    glutSetWindow(window2);
    glutPostRedisplay();
}