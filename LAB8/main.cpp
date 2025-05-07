#define GRID_SIZE_1 7
#define GRID_SIZE_2 10
#define GRID_SIZE_3 15
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>

int size = GRID_SIZE_1;
int filledGrid[GRID_SIZE_3 * 2 + 1][GRID_SIZE_3 * 2 + 1] = {0};

struct Point {
    int x, y;
};
Point endpoints[3]; 
int endpointCount = 0;

void ChangeSize(int, int);
void RenderScene(void);
void MouseClick(int button, int state, int x, int y);
void Menu(int index);
void DrawLine(void);
void FillClickBlank(void);
void DrawEndpoints(void);
void DrawLine(void);
void DrawMidPointLine(Point p0, Point p1);
void DrawMidPointLines(void);
void FillTriangle(void); 
int IsLeftSide(Point p, Point p0, Point p1); 

void DrawLine(void) {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = -size - 1; i <= size; i++) {
        glBegin(GL_LINES);
        glVertex2f(-size - 0.5f, i + 0.5f);
        glVertex2f( size + 0.5f, i + 0.5f);
        glEnd();
    }
    for (int i = -size - 1; i <= size; i++) {
        glBegin(GL_LINES);
        glVertex2f(i + 0.5f, -size - 0.5f);
        glVertex2f(i + 0.5f,  size + 0.5f);
        glEnd();
    }
}

void FillClickBlank(void) {
    glColor3f(0.0f, 0.5f, 1.0f);
    for (int i = 0; i < size * 2 + 1; i++) {
        for (int j = 0; j < size * 2 + 1; j++) {
            if (filledGrid[i][j]) {
                float x1 = i - size - 0.5f;
                float y1 = j - size - 0.5f;
                glBegin(GL_QUADS);
                  glVertex2f(x1,     y1);
                  glVertex2f(x1 + 1, y1);
                  glVertex2f(x1 + 1, y1 + 1);
                  glVertex2f(x1,     y1 + 1);
                glEnd();
            }
        }
    }
}

void DrawEndpoints(void) {
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int k = 0; k < endpointCount; k++) {
        float x1 = endpoints[k].x - size - 0.5f;
        float y1 = endpoints[k].y - size - 0.5f;
        glBegin(GL_QUADS);
          glVertex2f(x1,     y1);
          glVertex2f(x1 + 1, y1);
          glVertex2f(x1 + 1, y1 + 1);
          glVertex2f(x1,     y1 + 1);
        glEnd();
    }
}

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

int IsLeftSide(Point p, Point p0, Point p1) {
    return ((p1.x - p0.x) * (p.y - p0.y) - (p.x - p0.x) * (p1.y - p0.y));
}

void FillTriangle(void) {
    if (endpointCount == 3) {
        glColor3f(1.0f, 1.0f, 1.0f);
        
        int minX = endpoints[0].x, minY = endpoints[0].y;
        int maxX = endpoints[0].x, maxY = endpoints[0].y;
        
        for (int i = 1; i < 3; i++) {
            if (endpoints[i].x < minX) minX = endpoints[i].x;
            if (endpoints[i].y < minY) minY = endpoints[i].y;
            if (endpoints[i].x > maxX) maxX = endpoints[i].x;
            if (endpoints[i].y > maxY) maxY = endpoints[i].y;
        }
        
        minX = (minX < 0) ? 0 : minX;
        minY = (minY < 0) ? 0 : minY;
        maxX = (maxX >= 2*size+1) ? 2*size : maxX;
        maxY = (maxY >= 2*size+1) ? 2*size : maxY;
        
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                Point p = {x, y};
                
                int side1 = IsLeftSide(p, endpoints[0], endpoints[1]);
                int side2 = IsLeftSide(p, endpoints[1], endpoints[2]);
                int side3 = IsLeftSide(p, endpoints[2], endpoints[0]);
                
                if ((side1 >= 0 && side2 >= 0 && side3 >= 0) || 
                    (side1 <= 0 && side2 <= 0 && side3 <= 0)) {
                    
                    float x1 = x - size - 0.5f;
                    float y1 = y - size - 0.5f;
                    glBegin(GL_QUADS);
                        glVertex2f(x1,     y1);
                        glVertex2f(x1 + 1, y1);
                        glVertex2f(x1 + 1, y1 + 1);
                        glVertex2f(x1,     y1 + 1);
                    glEnd();
                }
            }
        }
    }
}

void DrawMidPointLine(Point p0, Point p1) {
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int abs_dx = abs(dx), abs_dy = abs(dy);
    int signX = (dx >= 0) ? 1 : -1;
    int signY = (dy >= 0) ? 1 : -1;

    int steps, p, incE, incNE;
    int x = p0.x, y = p0.y;
    if (abs_dx > abs_dy) {
        steps = abs_dx;
        p      = 2*abs_dy - abs_dx;
        incE   = 2*abs_dy;
        incNE  = 2*(abs_dy - abs_dx);
        for (int k = 0; k <= steps; k++) {
            if (k == 0) glColor3f(1, 0, 0);
            else if (p < 0) {            
                glColor3f(0, 1, 0);      
                p += incE; x += signX;
            }
            else {                       
                glColor3f(0, 0, 1);      
                p += incNE; x += signX; y += signY;
            }
            float x1 = x - size - 0.5f;
            float y1 = y - size - 0.5f;
            glBegin(GL_QUADS);
              glVertex2f(x1,     y1);
              glVertex2f(x1 + 1, y1);
              glVertex2f(x1 + 1, y1 + 1);
              glVertex2f(x1,     y1 + 1);
            glEnd();
        }
    } else {
        steps = abs_dy;
        p      = 2*abs_dx - abs_dy;
        incE   = 2*abs_dx;
        incNE  = 2*(abs_dx - abs_dy);
        for (int k = 0; k <= steps; k++) {
            if (k == 0) glColor3f(1, 0, 0);
            else if (p < 0) {
                glColor3f(0, 1, 0);
                p += incE; y += signY;
            }
            else {
                glColor3f(0, 0, 1);
                p += incNE; x += signX; y += signY;
            }
            float x1 = x - size - 0.5f;
            float y1 = y - size - 0.5f;
            glBegin(GL_QUADS);
              glVertex2f(x1,     y1);
              glVertex2f(x1 + 1, y1);
              glVertex2f(x1 + 1, y1 + 1);
              glVertex2f(x1,     y1 + 1);
            glEnd();
        }
    }
}

void DrawMidPointLines(void) {
    if (endpointCount == 3) {
        for (int i = 0; i < 3; i++) {
            DrawMidPointLine(
              endpoints[i],
              endpoints[(i+1)%3]
            );
        }
    }
}

void RenderScene(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-size-2, size+2, -size-2, size+2, -10, 10);

    DrawLine();
    FillTriangle(); 
    FillClickBlank();
    DrawMidPointLines();
    DrawEndpoints();

    glutSwapBuffers();
}

void MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLdouble model[16], proj[16];
        GLint vp[4];
        GLdouble wx, wy, wz;
        glGetDoublev(GL_MODELVIEW_MATRIX,  model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT,         vp);
        gluUnProject(x, vp[3]-y, 0, model, proj, vp, &wx, &wy, &wz);

        int gx = (int)round(wx + size);
        int gy = (int)round(wy + size);
        if (gx>=0 && gx<2*size+1 && gy>=0 && gy<2*size+1) {
            if (endpointCount < 3) {
                endpoints[endpointCount++] = (Point){gx, gy};
            }
            else {
                filledGrid[gx][gy] = 1;
            }
            glutPostRedisplay();
        }
    }
}

void Menu(int index) {
    switch (index) {
      case 1: size = GRID_SIZE_1; break;
      case 2: size = GRID_SIZE_2; break;
      case 3: size = GRID_SIZE_3; break;
    }

    endpointCount = 0;
    for (int i = 0; i < size*2+1; i++)
      for (int j = 0; j < size*2+1; j++)
        filledGrid[i][j] = 0;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("113598017 - LAB8");
    glutCreateMenu(Menu);
    glutAddMenuEntry("7x7",  1);
    glutAddMenuEntry("10x10",2);
    glutAddMenuEntry("15x15",3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutMouseFunc(MouseClick);
    glutMainLoop();
    return 0;
}