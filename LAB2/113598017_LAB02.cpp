#include <stdio.h>
#include <GL/freeglut.h>

void RenderScene(void);
void ChangeSize(int, int);
void RenderTrianglePrism();
void Update();

GLfloat x_angle = 0.1f;
GLfloat y_angle = 0.1f;
GLfloat z_angle = 0.1f;

GLfloat vertices[][3] = {
    {0.0f, 5.0f, 0.0f},
    {-5.0f, 0.0f, 0.0f},
    {5.0f, 0.0f, 0.0f},
    {0.0f, 5.0f, -5.0f},
    {5.0f, 0.0f, -5.0f},
    {-5.0f, 0.0f, -5.0f}
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
    glutCreateWindow("LAB 2");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutIdleFunc(Update);
    glutMainLoop();
    return 0;
}

void ChangeSize(int w, int h) {
    printf("Window Size= %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20, 20, -20, 20, -20, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderScene(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(x_angle, 1, 0, 0);
    glRotatef(y_angle, 0, 1, 0);
    glRotatef(x_angle, 0, 0, 1);
    RenderTrianglePrism();
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

void RenderTrianglePrism() {
    glBegin(GL_TRIANGLES); // Front
    glColor3fv(colors[0]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glEnd();

    glBegin(GL_TRIANGLES); // Left
    glColor3fv(colors[1]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[1]);
    glEnd();

    glBegin(GL_TRIANGLES); // Right
    glColor3fv(colors[2]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[2]);
    glEnd();

    glBegin(GL_TRIANGLES); // Back
    glColor3fv(colors[3]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glEnd();

    glBegin(GL_TRIANGLES); // Bottom
    glColor3fv(colors[4]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[4]);
    glEnd();
}