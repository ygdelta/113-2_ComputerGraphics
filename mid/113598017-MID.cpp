#define _USE_MATH_DEFINES
#define WIDTH 800
#define HEIGHT 600
#include <iostream>
#include <GL/freeglut.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

const int MODE_POINT = 1;
const int MODE_LINE = 2;
const int MODE_FACE = 3;
const int COLOR_SINGLE = 1;
const int COLOR_RANDOM = 2;
const string OBJ_FILE_PATH = "C:/Users/TingYu/Code/OpenGL/mid";
string customPath = "";

struct Vector3 {
    GLfloat x, y, z;
    float color[3];
    Vector3() : x(0), y(0), z(0) {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
    }
    Vector3(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
    }
    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3 operator*(float scale) const {
        return Vector3(x * scale, y * scale, z * scale);
    }
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    Vector3 normalize() const {
        float len = sqrt(x * x + y * y + z * z);
        return Vector3(x / len, y / len, z / len);
    }
    float len() const {
        return sqrt(x * x + y * y + z * z);
    }
};

struct Face {
    int indices[3];
    float color[3];
    Face() {
        for (int i = 0; i < 3; i++) {
            indices[i] = 0;
            color[i] = 1.0f;
        }
    }
};

struct Camera {
    Vector3 eye, center, up, forward, right;
    float distance;
    float theta, phi;
    Camera() {
        eye.z = 0.0f;
        up.x = 0.0f; up.y = 1.0f; up.z =0.0f;
        forward.x = 0.0f; forward.y = 0.0f; forward.z = -1.0f;
        right.x = 1.0f; right.y = 0.0f; right.z = 0.0f;
        distance = 0.0f;
        theta = 0.0f;
        phi = 0.0f;
    }
};

// Global variables
vector<Vector3> vertices;
vector<Face> faces;
Vector3 minPoint, maxPoint;
Camera camera;
int lastPoint[2] = {-1, -1}; // Last mouse position
float rotationFactor = 1.3f;
int renderMode = MODE_POINT;
int colorMode = COLOR_SINGLE;
int mouseButton = -1;
string filePath = OBJ_FILE_PATH + "/teapot.obj";

// Functions
void RenderInit();
void Render();
void RenderPoint();
void RenderLine();
void RenderFace();
void SetColor();
void NewMenu();
void MainMenu(int value);
void RenderModeMenu(int value);
void RenderColorMenu(int value);
void RenderObjectMenu(int value);
void Mouse(int button, int state, int x, int y);
void MouseDrag(int x, int y);
void UpdateCamera();
void AdjustPosition();
bool LoadObj(const string fileName);

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Midterm Project - 113598017");
    NewMenu();
    glutDisplayFunc(Render);
    glutMouseFunc(Mouse);
    glutMotionFunc(MouseDrag);
    if (argc > 1) {
        customPath = argv[1];
        LoadObj(customPath);
    }
    else {
        filePath = OBJ_FILE_PATH + "/teapot.obj";
        LoadObj(filePath);
    }
    glutMainLoop();
    return 0;
}

void NewMenu() {
    int renderModeMenu = glutCreateMenu(RenderModeMenu);
    glutAddMenuEntry("Point", 1);
    glutAddMenuEntry("Line", 2);
    glutAddMenuEntry("Face", 3);
    int renderColorMenu = glutCreateMenu(RenderColorMenu);
    glutAddMenuEntry("Single Color", 1);
    glutAddMenuEntry("Random Color", 2);
    int objectMenu = glutCreateMenu(RenderObjectMenu);
    glutAddMenuEntry("Teapot", 1);
    glutAddMenuEntry("Teddy", 2);
    glutAddMenuEntry("Octahedron", 3);
    glutAddMenuEntry("Gourd", 4);
    int mainMenu = glutCreateMenu(MainMenu);
    glutAddSubMenu("Render Mode", 1);
    glutAddSubMenu("Render Color", 2);
    glutAddSubMenu("Object", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void MainMenu(int value) {
    switch(value) {
    case 1:
        break;
    case 2:
        break;
    }
}

void RenderModeMenu(int value) {
    switch(value) {
    case 1:
        renderMode = MODE_POINT;
        break;
    case 2:
        renderMode = MODE_LINE;
        break;
    case 3:
        renderMode = MODE_FACE;
        break;
    }
    glutPostRedisplay();
}

void RenderColorMenu(int value) {
    switch(value) {
    case 1:
        colorMode = COLOR_SINGLE;
        break;
    case 2:
        colorMode = COLOR_RANDOM;
        break;
    }
    SetColor();
    glutPostRedisplay();
}

void RenderObjectMenu(int value) {
    switch(value) {
    case 1:
        filePath = OBJ_FILE_PATH + "/teapot.obj";
        break;
    case 2:
        filePath = OBJ_FILE_PATH + "/teddy.obj";
        break;
    case 3:
        filePath = OBJ_FILE_PATH + "/octahedron.obj";
        break;
    case 4:
        filePath = OBJ_FILE_PATH + "/gourd.obj";
        break;
    }
    LoadObj(filePath);
    SetColor();
    glutPostRedisplay();
}

void RenderAxis() {
    glBegin(GL_LINES);
    // X軸 (紅色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(20.0f, 0.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f, 0.0f);

    // Y軸 (綠色)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 20.0f, 0.0f);
    glVertex3f(0.0f, -20.0f, 0.0f);

    // Z軸 (藍色)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 20.0f);
    glVertex3f(0.0f, 0.0f, -20.0f);

    // 不繪製預設的任意旋轉軸，由各viewport各自繪製
    glEnd();
}

void RenderInit() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(72.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 300.0f);
    gluLookAt(
        camera.eye.x, camera.eye.y, camera.eye.z,
        camera.center.x, camera.center.y, camera.center.z,
        camera.up.x, camera.up.y, camera.up.z
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Render() {
    RenderInit();
    // test here
    switch(renderMode) {
    case MODE_POINT:
        RenderPoint();
        break;
    case MODE_LINE:
        RenderLine();
        break;
    case MODE_FACE:
        RenderFace();
        break;
    }
    glutSwapBuffers();
}

void RenderPoint() {
    glBegin(GL_POINTS);
    for (const auto& vertex : vertices) {
        glColor3fv(vertex.color);
        glPointSize(150.0);
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
}

void RenderLine() {
    glBegin(GL_LINES);
    for (const auto& face: faces) {
        glColor3fv(face.color);
        for (int i = 0; i < 3; i++) {
            glVertex3f(vertices[face.indices[i]].x, vertices[face.indices[i]].y, vertices[face.indices[i]].z);
            glVertex3f(vertices[face.indices[(i + 1) % 3]].x, vertices[face.indices[(i + 1) % 3]].y, vertices[face.indices[(i + 1) % 3]].z);
        }
    }
    glEnd();
}

void RenderFace() {
    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        glColor3fv(face.color);
        for (int i = 0; i < 3; i++) {
            glVertex3f(vertices[face.indices[i]].x, vertices[face.indices[i]].y, vertices[face.indices[i]].z);
        }
    }
    glEnd();
}

void SetColor() {
    for (auto& vertex : vertices) {
        if (colorMode == COLOR_SINGLE) {
            vertex.color[0] = vertex.color[1] = vertex.color[2] = 1.0f;
        }
        else if (colorMode == COLOR_RANDOM) {
            vertex.color[0] = static_cast<float>(rand()) / RAND_MAX;
            vertex.color[1] = static_cast<float>(rand()) / RAND_MAX;
            vertex.color[2] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
    for (auto& face : faces) {
        if (colorMode == COLOR_SINGLE) {
            face.color[0] = face.color[1] = face.color[2] = 1.0f;
        }
        else if (colorMode == COLOR_RANDOM) {
            face.color[0] = static_cast<float>(rand()) / RAND_MAX;
            face.color[1] = static_cast<float>(rand()) / RAND_MAX;
            face.color[2] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}

bool LoadObj(const string fileName) {
    vertices.clear();
    faces.clear();
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        return false;
    }
    minPoint = maxPoint = Vector3(0.0f, 0.0f, 0.0f);
    string line;
    string prefix;
    while (getline(file, line)) {
        istringstream iss(line);
        iss >> prefix;
        if (prefix == "v") {
            Vector3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
            if (vertex.x < minPoint.x) minPoint.x = vertex.x;
            if (vertex.y < minPoint.y) minPoint.y = vertex.y;
            if (vertex.z < minPoint.z) minPoint.z = vertex.z;
            if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
            if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
            if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
        }
        else if (prefix == "f") {
            Face face;
            for (int i = 0; i < 3; i++) {
                iss >> face.indices[i];
                face.indices[i]--; // OBJ format uses 1-based indexing
            }
            faces.push_back(face);
        }
    }
    file.close();
    AdjustPosition();
    return true;
}

void Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastPoint[0] = x;
        lastPoint[1] = y;   
        mouseButton = GLUT_LEFT_BUTTON;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        lastPoint[0] = x;
        lastPoint[1] = y;
        mouseButton = GLUT_MIDDLE_BUTTON;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        lastPoint[0] = lastPoint[1] = -1;
        mouseButton = -1;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
        lastPoint[0] = lastPoint[1] = -1;
        mouseButton = -1;
    }
    else if (button == 3) {
        lastPoint[0] = lastPoint[1] = -1;
        camera.distance -= 1.5f * camera.distance * 0.01;
        UpdateCamera();
        glutPostRedisplay();
    }
    else if (button == 4) {
        lastPoint[0] = lastPoint[1] = -1;
        camera.distance += 1.5f * camera.distance * 0.01;
        UpdateCamera();
        glutPostRedisplay();
    }
}

void MouseDrag(int x, int y) {
    int dx = x - lastPoint[0];
    int dy = y - lastPoint[1];
    if (mouseButton == GLUT_LEFT_BUTTON) {
        camera.theta -= dx * rotationFactor;
        camera.phi += dy * rotationFactor;
        if (camera.theta < -360.0f) camera.theta += 360.0f;
        if (camera.phi >= 89.0f) {
            camera.phi = 89.0f;
        }
        else if (camera.phi <= -89.0f) {
            camera.phi = -89.0f;
        }
        UpdateCamera();
        camera.forward = (camera.center - camera.eye).normalize();
        camera.right = camera.forward.cross(Vector3(0.0f, 1.0f, 0.0f)).normalize();
        camera.up = camera.right.cross(camera.forward).normalize();
    }
    else if (mouseButton == GLUT_MIDDLE_BUTTON) {
        camera.eye = camera.eye + camera.right * dx;
        camera.eye = camera.eye - camera.up * dy;
        camera.center = camera.center + camera.right * dx;
        camera.center = camera.center - camera.up * dy;
        // Handle right mouse button down
    }
    lastPoint[0] = x;
    lastPoint[1] = y;
    glutPostRedisplay();
}

void UpdateCamera() {
    camera.eye.x = camera.center.x + camera.distance * cos(camera.phi * M_PI / 180.0f) * sin(camera.theta * M_PI / 180.0f);
    camera.eye.y = camera.center.y + camera.distance * sin(camera.phi * M_PI / 180.0f);
    camera.eye.z = camera.center.z + camera.distance * cos(camera.theta * M_PI / 180.0f) * cos(camera.phi * M_PI / 180.0f);
}

void AdjustPosition() {
    if (vertices.empty()) return;
    if (faces.empty()) return;
    camera = Camera(); // Reset camera
    RenderInit();
    int viewPort[4];
    double projMatrix[16];
    double modelMatrix[16];
    double x1, y1, z1, x2, y2, z2;
    while (1) {
        glGetIntegerv(GL_VIEWPORT, viewPort);
        glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        gluProject(minPoint.x, minPoint.y, minPoint.z, modelMatrix, projMatrix, viewPort, &x1, &y1, &z1);
        gluProject(maxPoint.x, maxPoint.y, maxPoint.z, modelMatrix, projMatrix, viewPort, &x2, &y2, &z2);
        if (x1 > 0 && y1 > 0 && x2 > 0 && y2 > 0 && x1 < WIDTH && y1 < HEIGHT && x2 < WIDTH && y2 < HEIGHT) {
            break;
        }
		camera.eye.z += 1.0f;
		RenderInit();
    }
    camera.distance = camera.eye.z;
    glutPostRedisplay();
}