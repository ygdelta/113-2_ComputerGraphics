#define GRID_SIZE_1 7
#define GRID_SIZE_2 10
#define GRID_SIZE_3 15
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>
#include <time.h>  

struct Color {
    float r, g, b;
};

struct Point {
    int x, y;
    Color color;  
};

int size = GRID_SIZE_1;
int filledGrid[GRID_SIZE_3 * 2 + 1][GRID_SIZE_3 * 2 + 1] = {0};
Color colorGrid[GRID_SIZE_3 * 2 + 1][GRID_SIZE_3 * 2 + 1]; 

Point endpoints[10];  
int endpointCount = 0;

Point* fillPoints = NULL;
int fillPointCount = 0;
int currentFillPoint = 0;
int animationActive = 0;
int animationDelay = 50;  

int showMidpointLines = 1;  
int activeVertex = -1;      

void ChangeSize(int, int);
void RenderScene(void);
void MouseClick(int button, int state, int x, int y);
void Menu(int index);
void DrawGrid(void);
void FillClickBlank(void);
void DrawEndpoints(void);
void DrawMidPointLine(Point p0, Point p1);
void DrawMidPointLines(void);
void PrepareCrowFilling(void);
void AnimationTimer(int value);
void KeyboardFunc(unsigned char key, int x, int y);
void SpecialKeyFunc(int key, int x, int y);
void resetAll(void);

void fillPolygon(Point vList[], int n);
Color interpolateColor(Color c1, Color c2, float t);

Color randomColor() {
    Color c;
    c.r = (float)rand() / RAND_MAX;
    c.g = (float)rand() / RAND_MAX;
    c.b = (float)rand() / RAND_MAX;
    return c;
}

Color interpolateColor(Color c1, Color c2, float t) {
    Color result;
    result.r = c1.r + t * (c2.r - c1.r);
    result.g = c1.g + t * (c2.g - c1.g);
    result.b = c1.b + t * (c2.b - c1.b);
    return result;
}

void resetAll() {
    endpointCount = 0;
    activeVertex = -1;
    
    for (int i = 0; i < GRID_SIZE_3*2+1; i++) {
        for (int j = 0; j < GRID_SIZE_3*2+1; j++) {
            filledGrid[i][j] = 0;
        }
    }
    
    if (fillPoints != NULL) {
        free(fillPoints);
        fillPoints = NULL;
    }
    
    fillPointCount = 0;
    currentFillPoint = 0;
    animationActive = 0;
}

void DrawGrid(void) {
    glColor3f(0.3f, 0.3f, 0.3f);  
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
    for (int i = 0; i < 2*size+1; i++) {
        for (int j = 0; j < 2*size+1; j++) {
            if (filledGrid[i][j]) {
                glColor3f(colorGrid[i][j].r, colorGrid[i][j].g, colorGrid[i][j].b);
                
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
    for (int k = 0; k < endpointCount; k++) {
        if (k == activeVertex) {
            glColor3f(1.0f, 1.0f, 0.0f);  
        } else {
            glColor3f(endpoints[k].color.r, endpoints[k].color.g, endpoints[k].color.b);
        }
        
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

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void fillPolygon(Point vertices[], int n) {
    if (n < 3) return; 
    
    int minY = vertices[0].y;
    int maxY = vertices[0].y;
    
    for (int i = 1; i < n; i++) {
        if (vertices[i].y < minY) minY = vertices[i].y;
        if (vertices[i].y > maxY) maxY = vertices[i].y;
    }
    
    int* intersectX = (int*)malloc(n * sizeof(int));
    Color* intersectColors = (Color*)malloc(n * sizeof(Color));
    if (!intersectX || !intersectColors) {
        if (intersectX) free(intersectX);
        if (intersectColors) free(intersectColors);
        return;
    }
    
    for (int y = minY; y <= maxY; y++) {
        int intersectCount = 0;
        
        for (int i = 0; i < n; i++) {
            int nextI = (i + 1) % n;
            int y1 = vertices[i].y;
            int y2 = vertices[nextI].y;
            
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                float t = (float)(y - y1) / (y2 - y1);
                int x = vertices[i].x + t * (vertices[nextI].x - vertices[i].x);
                Color color;
                color.r = 1.0;
                color.g = 1.0;
                color.b = 1.0;
                
                intersectX[intersectCount] = x;
                intersectColors[intersectCount] = color;
                intersectCount++;
            }
        }
        
        for (int i = 0; i < intersectCount - 1; i++) {
            for (int j = 0; j < intersectCount - i - 1; j++) {
                if (intersectX[j] > intersectX[j + 1]) {
                    int tempX = intersectX[j];
                    intersectX[j] = intersectX[j + 1];
                    intersectX[j + 1] = tempX;
                    
                    Color tempColor = intersectColors[j];
                    intersectColors[j] = intersectColors[j + 1];
                    intersectColors[j + 1] = tempColor;
                }
            }
        }
        
        for (int i = 0; i < intersectCount; i += 2) {
            if (i + 1 < intersectCount) {
                int xStart = intersectX[i];
                int xEnd = intersectX[i + 1];
                
                Color colorStart = intersectColors[i];
                Color colorEnd = intersectColors[i + 1];
                
                for (int x = xStart; x <= xEnd; x++) {
                    float t = (xEnd == xStart) ? 0 : (float)(x - xStart) / (xEnd - xStart);
                    Color pixelColor;
                    pixelColor.r = colorStart.r + t * (colorEnd.r - colorStart.r);
                    pixelColor.g = colorStart.g + t * (colorEnd.g - colorStart.g);
                    pixelColor.b = colorStart.b + t * (colorEnd.b - colorStart.b);
                    
                    if (x >= 0 && x < 2*size+1 && y >= 0 && y < 2*size+1) {
                        Point p = {x, y, pixelColor};
                        if (fillPointCount < (2*size+1) * (2*size+1) && fillPoints != NULL) {
                            fillPoints[fillPointCount++] = p;
                        }
                    }
                }
            }
        }
    }
    
    free(intersectX);
    free(intersectColors);
}

void PrepareCrowFilling(void) {
    if (endpointCount >= 3) {
        if (fillPoints != NULL) {
            free(fillPoints);
            fillPoints = NULL;
        }
        
        for (int i = 0; i < 2*size+1; i++) {
            for (int j = 0; j < 2*size+1; j++) {
                filledGrid[i][j] = 0;
            }
        }
        
        fillPoints = (Point*)malloc((2*size+1) * (2*size+1) * sizeof(Point));
        if (fillPoints == NULL) return;
        
        fillPointCount = 0;
        currentFillPoint = 0;
        
        fillPolygon(endpoints, endpointCount);
        
        animationActive = 1;
        glutTimerFunc(animationDelay, AnimationTimer, 0);
    }
}

void AnimationTimer(int value) {
    if (animationActive && currentFillPoint < fillPointCount) {
        int pointsPerFrame = 10;
        for (int i = 0; i < pointsPerFrame && currentFillPoint < fillPointCount; i++) {
            Point p = fillPoints[currentFillPoint];
            
            if (p.x >= 0 && p.x < 2*size+1 && p.y >= 0 && p.y < 2*size+1) {
                filledGrid[p.x][p.y] = 1;
                colorGrid[p.x][p.y] = p.color;
            }
            
            currentFillPoint++;
        }
        
        glutPostRedisplay();
        
        glutTimerFunc(animationDelay, AnimationTimer, 0);
    } else {
        animationActive = 0;
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
            float t = (float)k / steps;
            Color pixelColor = interpolateColor(p0.color, p1.color, t);
            glColor3f(pixelColor.r, pixelColor.g, pixelColor.b);
            
            float x1 = x - size - 0.5f;
            float y1 = y - size - 0.5f;
            glBegin(GL_QUADS);
              glVertex2f(x1,     y1);
              glVertex2f(x1 + 1, y1);
              glVertex2f(x1 + 1, y1 + 1);
              glVertex2f(x1,     y1 + 1);
            glEnd();
            
            if (p < 0) {            
                p += incE; x += signX;
            }
            else {                       
                p += incNE; x += signX; y += signY;
            }
        }
    } else {
        steps = abs_dy;
        p      = 2*abs_dx - abs_dy;
        incE   = 2*abs_dx;
        incNE  = 2*(abs_dx - abs_dy);
        for (int k = 0; k <= steps; k++) {
            float t = (float)k / steps;
            Color pixelColor = interpolateColor(p0.color, p1.color, t);
            glColor3f(pixelColor.r, pixelColor.g, pixelColor.b);
            
            float x1 = x - size - 0.5f;
            float y1 = y - size - 0.5f;
            glBegin(GL_QUADS);
              glVertex2f(x1,     y1);
              glVertex2f(x1 + 1, y1);
              glVertex2f(x1 + 1, y1 + 1);
              glVertex2f(x1,     y1 + 1);
            glEnd();
            
            if (p < 0) {
                p += incE; y += signY;
            }
            else {
                p += incNE; x += signX; y += signY;
            }
        }
    }
}

void DrawMidPointLines(void) {
    if (showMidpointLines && endpointCount >= 3) {
        for (int i = 0; i < endpointCount; i++) {
            DrawMidPointLine(
              endpoints[i],
              endpoints[(i+1) % endpointCount]
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

    DrawGrid();
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
            for (int i = 0; i < endpointCount; i++) {
                if (endpoints[i].x == gx && endpoints[i].y == gy) {
                    activeVertex = i;  
                    glutPostRedisplay();
                    return;
                }
            }
            
            if (endpointCount < 10) {  
                Point newPoint = {gx, gy, randomColor()};  
                endpoints[endpointCount++] = newPoint;
                
                activeVertex = endpointCount - 1;
                
                if (endpointCount >= 3) {
                    PrepareCrowFilling();
                }
            }
            glutPostRedisplay();
        }
    }
}

void KeyboardFunc(unsigned char key, int x, int y) {
  
}

void SpecialKeyFunc(int key, int x, int y) {
    if (activeVertex >= 0 && activeVertex < endpointCount) {
        switch (key) {
            case GLUT_KEY_UP:
                if (endpoints[activeVertex].y < 2*size)
                    endpoints[activeVertex].y++;
                break;
            case GLUT_KEY_DOWN:
                if (endpoints[activeVertex].y > 0)
                    endpoints[activeVertex].y--;
                break;
            case GLUT_KEY_LEFT:
                if (endpoints[activeVertex].x > 0)
                    endpoints[activeVertex].x--;
                break;
            case GLUT_KEY_RIGHT:
                if (endpoints[activeVertex].x < 2*size)
                    endpoints[activeVertex].x++;
                break;
        }
        
        if (endpointCount >= 3) {
            PrepareCrowFilling();
        }
        
        glutPostRedisplay();
    }
}

void Menu(int index) {
    resetAll();
    
    switch (index) {
      case 1: size = GRID_SIZE_1; break;
      case 2: size = GRID_SIZE_2; break;
      case 3: size = GRID_SIZE_3; break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    srand(time(NULL));  
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("113598017-LAB10");
    
    glutCreateMenu(Menu);
    glutAddMenuEntry("7x7",  1);
    glutAddMenuEntry("10x10", 2);
    glutAddMenuEntry("15x15", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutMouseFunc(MouseClick);
    glutKeyboardFunc(KeyboardFunc);
    glutSpecialFunc(SpecialKeyFunc);
    
    glutMainLoop();
    
    if (fillPoints != NULL) {
        free(fillPoints);
        fillPoints = NULL;
    }
    
    return 0;
}