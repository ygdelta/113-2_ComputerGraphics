#include <opencv2/opencv.hpp>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

GLuint textureIDs[3]; 
int windowWidth = 800, windowHeight = 600;

vector<string> texturePaths = {
    "textures/Block6.jpg",  
    "textures/Block5.jpg",  
    "textures/Block4.jpg"     
};

GLuint loadTexture(const string& filename) {
    Mat image = imread(filename, IMREAD_COLOR);
    if (image.empty()) {
        cout << "cannot open img: " << filename << endl;
		return -1;
    }
    
    cvtColor(image, image, COLOR_BGR2RGB);
    
    flip(image, image, 0);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    
    return textureID;
}

void initTextures() {
    for (int i = 0; i < 3; i++) {
        if (i < texturePaths.size()) {
            textureIDs[i] = loadTexture(texturePaths[i]);
        }
    }
}

void drawQuad() {
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();
}

void drawCube() {
    glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f);
    drawQuad();
    glPopMatrix();
    
    glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawQuad();
    glPopMatrix();
    
    glBindTexture(GL_TEXTURE_2D, textureIDs[2]);
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawQuad();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(4.0, 3.0, 4.0,    
              0.0, 0.0, 0.0,    
              0.0, 1.0, 0.0);   
    
    drawCube();
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// 鍵盤回調函數
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC鍵退出
            exit(0);
            break;
        case 'r':
        case 'R': // R鍵重新載入紋理
            initTextures();
            glutPostRedisplay();
            break;
    }
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    
    GLfloat matAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("113598017 - LAB11");
    
    initGL();
    initTextures();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    // glutKeyboardFunc(keyboard);
    
    
    glutMainLoop();
    
    return 0;
}