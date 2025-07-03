#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;

GLfloat axis[3] = { 0.0f, 0.0f, 0.0f };
GLfloat v1[3] = { 5.0f, 10.0f, -5.0f };
GLfloat v2[3] = { -5.0f, -10.0f, 5.0f };

GLfloat vertices[][3] = {
    {-1.25, -1.25, -1.25},  // 頂點0: 左下後
    {1.25, -1.25, -1.25},   // 頂點1: 右下後
    {1.25, 1.25, -1.25},    // 頂點2: 右上後
    {-1.25, 1.25, -1.25},   // 頂點3: 左上後
    {-1.25, -1.25, 1.25},   // 頂點4: 左下前
    {1.25, -1.25, 1.25},    // 頂點5: 右下前
    {1.25, 1.25, 1.25},     // 頂點6: 右上前
    {-1.25, 1.25, 1.25}     // 頂點7: 左上前
};

GLfloat colors[][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f}
};

void Scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);
void Rotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void Translate(GLfloat tx, GLfloat ty, GLfloat tz);
void RenderScene();
void RenderCube();
void ChangeSize(int, int);
void keyBoard(unsigned char, int, int);
void RenderAxis();

GLfloat angle = 0.0f;
GLfloat x_prefix = 0.0f;
GLfloat y_prefix = 0.0f;
GLfloat z_prefix = 0.0f;
GLfloat x_scale = 1.0f;
GLfloat y_scale = 1.0f;
GLfloat z_scale = 1.0f;

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("LAB 4");
    if (argc == 7) {
        v1[0] = stof(argv[1]); v1[1] = stof(argv[2]); v1[2] = stof(argv[3]);
        v2[0] = stof(argv[4]); v2[1] = stof(argv[5]); v2[2] = stof(argv[6]);
    }
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keyBoard);
    // glutIdleFunc(Update);
    glutMainLoop();
    return 0;
}

void keyBoard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        angle -= 0.3f;
        if (angle < 0.0f) angle = 360.0f;
        break;
    case 'e':
        angle += 0.3f;
        if (angle > 360.0f) angle = 0.0f;
        break;
    case 'd':
        x_prefix += 0.5f;
        break;
    case 'a':
        x_prefix -= 0.5f;
        break;
    case ' ':
        y_prefix += 0.5f;
        break;
    case 'c':
        y_prefix -= 0.5f;
        break;
    case 'w':
        z_prefix += 0.5f;
        break;
    case 's':
        z_prefix -= 0.5f;
        break;
    case '1':
        x_scale += 0.5f;
        break;
    case '2':
        y_scale += 0.5f;
        break;
    case '3':
        z_scale += 0.5f;
        break;
    case '4':
        x_scale -= 0.5f;
        break;
    case '5':
        y_scale -= 0.5f;
        break;
    case '6':
        z_scale -= 0.5f;
        break;
    case 'r':
        angle = 0.0f;
        x_prefix = 0.0f;
        y_prefix = 0.0f;
        z_prefix = 0.0f;
        x_scale = 1.0f;
        y_scale = 1.0f;
        z_scale = 1.0f;
        break;
    }
    glutPostRedisplay();
}

void ChangeSize(int w, int h) {
    printf("Window Size = %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    RenderAxis();
    Rotation(angle, v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
    Translate(x_prefix, y_prefix, z_prefix);
    Scale(x_scale, y_scale, z_scale);
    RenderCube();
    glutSwapBuffers();
}

void RenderAxis() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(20.0f, 0.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 20.0f, 0.0f);
    glVertex3f(0.0f, -20.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 20.0f);
    glVertex3f(0.0f, 0.0f, -20.0f);
    // Arbitrary Rotation Axis
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glEnd();
}

void Scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
    GLfloat scaleMatrix[16] = {
        scaleX,   0.0f,   0.0f, 0.0f,
        0.0f  , scaleY,   0.0f, 0.0f,
        0.0f  ,   0.0f, scaleZ, 0.0f,
        0.0f  ,   0.0f,   0.0f, 1.0f
    };
    glMultMatrixf(scaleMatrix);
}

void Rotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat cos = std::cos(angle);
    GLfloat sin = std::sin(angle);
    GLfloat c = 1.0f - cos;
    GLfloat len = sqrt(x * x + y * y + z * z);
    x = x / len;
    y = y / len;
    z = z / len;

    GLfloat rotationMatrix[16] = {
        x * x * c + cos    , x * y * c + z * sin, x * z * c - y * sin, 0.0f,
        x * y * c - z * sin, y * y * c + cos    , y * z * c + x * sin, 0.0f,
        x * z * c + y * sin, y * z * c - x * sin, z * z * c + cos    , 0.0f,
        0.0f               , 0.0f               , 0.0f               , 1.0f
    };
    return glMultMatrixf(rotationMatrix);
}

void Translate(GLfloat tx, GLfloat ty, GLfloat tz) {
    GLfloat transMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        tx  , ty  , tz  , 1.0f
    };
    glMultMatrixf(transMatrix);
}

void RenderCube() {
    glBegin(GL_TRIANGLE_STRIP); // Front
    glColor3fv(colors[0]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP); // Left
    glColor3fv(colors[1]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[4]);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP); // Right
    glColor3fv(colors[2]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[1]);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP); // Back
    glColor3fv(colors[3]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[0]);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP); // Bottom
    glColor3fv(colors[4]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[5]);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP); // Top
    glColor3fv(colors[5]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[7]);
    glEnd();
}