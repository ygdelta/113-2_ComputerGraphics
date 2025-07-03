#include <GL/freeglut.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// Global variables
int windowWidth = 800;
int windowHeight = 800;
int gridDimension = 10; // Default dimension (10x10 grid)
int selectedDimension = 10; // For menu selection

// Coordinates range from -dimension/2 to dimension/2
float minX, maxX, minY, maxY;

// Cell tracking
struct Cell {
    int x;
    int y;
    bool filled;
};

std::vector<Cell> cells;

// Function prototypes
void display();
void reshape(int width, int height);
void mouse(int button, int state, int x, int y);
void initGrid();
void processMenuEvents(int option);
void createPopupMenu();
std::pair<int, int> screenToGrid(int x, int y);
void drawGridLines();
void drawFilledCells();

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lab6");

    // Set up callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    // Create menu
    createPopupMenu();

    // Initialize grid
    initGrid();

    // Enter main loop
    glutMainLoop();

    return 0;
}

void createPopupMenu() {
    int menu = glutCreateMenu(processMenuEvents);
    glutAddMenuEntry("10 x 10 Grid", 10);
    glutAddMenuEntry("15 x 15 Grid", 15);
    glutAddMenuEntry("20 x 20 Grid", 20);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processMenuEvents(int option) {
    selectedDimension = option;
    gridDimension = option;
    initGrid();
    glutPostRedisplay();
}

void initGrid() {
    cells.clear();
    minX = -gridDimension / 2;
    maxX = gridDimension / 2;
    minY = -gridDimension / 2;
    maxY = gridDimension / 2;
}

void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(minX - 0.5f, maxX + 0.5f, minY - 0.5f, maxY + 0.5f, -1.0, 1.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawFilledCells();
    drawGridLines();
    glutSwapBuffers();
}

void drawGridLines() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    
    for (int i = -gridDimension/2; i <= gridDimension/2; i++) {
        glBegin(GL_LINES);
        glVertex2f(i, -gridDimension/2);
        glVertex2f(i, gridDimension/2);
        glEnd();
    }
    
    for (int i = -gridDimension/2; i <= gridDimension/2; i++) {
        glBegin(GL_LINES);
        glVertex2f(-gridDimension/2, i);
        glVertex2f(gridDimension/2, i);
        glEnd();
    }
}

void drawFilledCells() {
    glColor3f(0.0f, 1.0f, 0.0f); // 綠色填充
    
    for (const auto& cell : cells) {
        if (cell.filled) {
            glBegin(GL_QUADS);
            glVertex2f(cell.x, cell.y);                   
            glVertex2f(cell.x + 1.0f, cell.y);            
            glVertex2f(cell.x + 1.0f, cell.y + 1.0f);     
            glVertex2f(cell.x, cell.y + 1.0f);            
            glEnd();
        }
    }
}

void reshape(int width, int height) {
    // Update window dimensions
    windowWidth = width;
    windowHeight = height;
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Redraw
    glutPostRedisplay();
}

std::pair<int, int> screenToGrid(int screenX, int screenY) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble projMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    
    GLdouble modelMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    
    GLdouble worldX, worldY, worldZ;
    gluUnProject(screenX, viewport[3] - screenY, 0.0, 
                 modelMatrix, projMatrix, viewport, 
                 &worldX, &worldY, &worldZ);

    int gridX = static_cast<int>(floor(worldX));
    int gridY = static_cast<int>(floor(worldY));    
    return std::make_pair(gridX, gridY);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        std::pair<int, int> gridCoords = screenToGrid(x, y);
        int gridX = gridCoords.first;
        int gridY = gridCoords.second;
        if (gridX >= minX && gridX < maxX && gridY >= minY && gridY < maxY) {
            bool cellFound = false;
            for (auto& cell : cells) {
                if (cell.x == gridX && cell.y == gridY) {
                    cell.filled = !cell.filled; 
                    cellFound = true;
                    break;
                }
            }
            if (!cellFound) {
                cells.push_back({gridX, gridY, true});
            }            
            glutPostRedisplay();
        }
    }
}