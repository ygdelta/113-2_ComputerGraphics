#include <stdio.h>
#include <GL/freeglut.h>

void RenderScene(void);
void ChangeSize(int, int);
void RenderCube();
void RenderAxis();
void Update();
void keyBoard(unsigned char, int, int);

GLfloat x_angle  = 10.0f;
GLfloat y_angle  = 10.0f;
GLfloat z_angle  = 10.0f;
GLfloat x_prefix = 0.0f;
GLfloat y_prefix = 0.0f;
GLfloat z_prefix = 0.0f;
GLfloat x_scale  = 1.0f;
GLfloat y_scale  = 1.0f;
GLfloat z_scale  = 1.0f;

GLfloat vertices[][3] = {
    {-2.5, -2.5, -2.5},  // 頂點0: 左下後
    {2.5, -2.5, -2.5},   // 頂點1: 右下後
    {2.5, 2.5, -2.5},    // 頂點2: 右上後
    {-2.5, 2.5, -2.5},   // 頂點3: 左上後
    {-2.5, -2.5, 2.5},   // 頂點4: 左下前
    {2.5, -2.5, 2.5},    // 頂點5: 右下前
    {2.5, 2.5, 2.5},     // 頂點6: 右上前
    {-2.5, 2.5, 2.5}     // 頂點7: 左上前
};

GLfloat colors[][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f}
};

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("LAB 3");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keyBoard);
    // glutIdleFunc(Update);
    glutMainLoop();
    return 0;
}

void keyBoard(unsigned char key, int x, int y) {
    switch(key) {
    case '1':
        if (x_angle > 360.0f) {
            x_angle = 0.0f;
        }
        x_angle += 2.0f;
        break;
    case '2':
        if (y_angle > 360.0f) {
            y_angle = 0.0f;
        }
        y_angle += 2.0f;
        break;
    case '3':
        if (z_angle > 360.0f) {
            z_angle = 0.0f;
        }
        z_angle += 2.0f;
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
    case '4':
        x_scale += 0.5f;
        break;
    case '5':
        y_scale += 0.5f;
        break;
    case '6':
        z_scale += 0.5f;
        break;
    case '7':
        x_scale -= 0.5f;
        break;
    case '8':
        y_scale -= 0.5f;
        break;
    case '9':
        z_scale -= 0.5f;
        break;
    case 'r':
        x_angle = 0.1f;
        y_angle = 0.1f;
        z_angle = 0.1f;
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
    printf("Window Size= %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(-20, 20, -20, 20, -20, 20);
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderScene(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0f, 10.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    RenderAxis();
    glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
    glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
    glRotatef(z_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(x_prefix, y_prefix, z_prefix);
    glScalef(x_scale, y_scale, z_scale);
    RenderCube();
    glutSwapBuffers();
}

void Update() {
    x_angle += 0.5f;
    y_angle += 0.3f;
    z_angle += 0.7f;
    if (x_angle > 360.0f) {
        x_angle = 0.5f;
    }
    if (y_angle > 360.0f) {
        y_angle = 0.3f;
    }
    if (z_angle > 360.0f) {
        z_angle = 0.7f;
    }
    glutPostRedisplay();
}

void RenderAxis() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f, 0.0f);
    glVertex3f(20.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -20.0f, 0.0f);
    glVertex3f(0.0f, 20.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -20.0f);
    glVertex3f(0.0f, 0.0f, 20.0f);
    glEnd();
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