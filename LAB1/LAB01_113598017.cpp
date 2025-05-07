#include <stdio.h> 
#include <stdlib.h>
/*** freeglut***/
#include <GL/freeglut.h>

void ChangeSize(int, int);
void RenderScene(void);
int _mode = GL_SMOOTH;
bool isBio = false;

void Menu(int index) {
    switch (index)
    {
    case 1:
        _mode = GL_SMOOTH;
        isBio = false;
        break;
    case 2:
        _mode = GL_FLAT;
        isBio = false;
        break;
    case 3:
        _mode = GL_SMOOTH;
        isBio = true;
        break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("Simple Triangle");

    glutCreateMenu(Menu);
    glutAddMenuEntry("smooth", 1);
    glutAddMenuEntry("flat", 2);
    glutAddMenuEntry("brindle", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutMainLoop();	//http://www.programmer-club.com.tw/ShowSameTitleN/opengl/2288.html
    return 0;
}

void ChangeSize(int w, int h) {
    printf("Window Size= %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void RenderScene(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(_mode);
    glLoadIdentity();
    gluLookAt(0, 0, 10.0f, 0, 0, 0, 0, 1, 0);
    glBegin(GL_POLYGON);

    if (!isBio) {
        glColor3f(0, 1, 1); glVertex3f(-2.5, 4.33, 0);
        glColor3f(0, 1, 0); glVertex3f(-5, 0, 0);
        glColor3f(0, 0, 1); glVertex3f(-2.5, -4.33, 0);
        glColor3f(1, 0, 0); glVertex3f(2.5, -4.33, 0);
        glColor3f(1, 1, 0); glVertex3f(5, 0, 0);
        glColor3f(1, 0, 1); glVertex3f(2.5, 4.33, 0);
    } 
    else {
        glColor3f(0, 0, 0); glVertex3f(-2.5, 4.33, 0);
        glColor3f(1, 1, 1); glVertex3f(-5, 0, 0);
        glColor3f(0, 0, 0); glVertex3f(-2.5, -4.33, 0);
        glColor3f(1, 1, 1); glVertex3f(2.5, -4.33, 0);
        glColor3f(0, 0, 0); glVertex3f(5, 0, 0);
        glColor3f(1, 1, 1); glVertex3f(2.5, 4.33, 0);
    }

    glEnd();
    glutSwapBuffers();
    glutPostRedisplay();
}