#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>
#define X 0.525731112119133606f
#define Z 0.850650808352039932f

int subdivision_depth = 0;
bool polygon_fill = true; 
float rotation_x = 0.0f, rotation_y = 0.0f;
int mouse_x, mouse_y;
bool mouse_left_down = false;
int window_width = 1200, window_height = 400;

static GLfloat vdata[12][3] = {
    {-X, 0.0f, Z}, {X, 0.0f, Z}, {-X, 0.0f, -Z}, {X, 0.0f, -Z},
    {0.0f, Z, X}, {0.0f, Z, -X}, {0.0f, -Z, X}, {0.0f, -Z, -X},
    {Z, X, 0.0f}, {-Z, X, 0.0f}, {Z, -X, 0.0f}, {-Z, -X, 0.0f}
};

static GLuint tindices[20][3] = {
    {1, 4, 0}, {4, 9, 0}, {4, 5, 9}, {8, 5, 4}, {1, 8, 4},
    {1, 10, 8}, {10, 3, 8}, {8, 3, 5}, {3, 2, 5}, {3, 7, 2},
    {3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11}, {6, 1, 0},
    {10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7}
};

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float f) const { return Vector3(x * f, y * f, z * f); }
    Vector3 cross(const Vector3& v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    float length() const { return sqrt(x * x + y * y + z * z); }
    Vector3 normalize() const { float len = length(); return len > 0 ? Vector3(x / len, y / len, z / len) : Vector3(); }
};

struct Triangle {
    Vector3 v1, v2, v3;
    Triangle(const Vector3& v1, const Vector3& v2, const Vector3& v3) : v1(v1), v2(v2), v3(v3) {}
};

std::vector<Triangle> triangles;

void normalize(float v[3]) {
    float d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (d == 0.0f) return;
    v[0] /= d; v[1] /= d; v[2] /= d;
}

void normCrossProd(float u[3], float v[3], float out[3]) {
    out[0] = u[1] * v[2] - u[2] * v[1];
    out[1] = u[2] * v[0] - u[0] * v[2];
    out[2] = u[0] * v[1] - u[1] * v[0];
    normalize(out);
}

void calculateNormal(int i, float normal[3]) {
    float d1[3], d2[3];
    
    for (int k = 0; k < 3; k++) {
        d1[k] = vdata[tindices[i][1]][k] - vdata[tindices[i][0]][k];
        d2[k] = vdata[tindices[i][2]][k] - vdata[tindices[i][0]][k];
    }
    normCrossProd(d1, d2, normal);
}

void subdivide(Vector3 v1, Vector3 v2, Vector3 v3, int depth) {
    if (depth == 0) {
        triangles.push_back(Triangle(v1, v2, v3));
        return;
    }
    
    Vector3 v12 = ((v1 + v2) * 0.5f).normalize();
    Vector3 v23 = ((v2 + v3) * 0.5f).normalize();
    Vector3 v31 = ((v3 + v1) * 0.5f).normalize();
    
    subdivide(v1, v12, v31, depth - 1);
    subdivide(v2, v23, v12, depth - 1);
    subdivide(v3, v31, v23, depth - 1);
    subdivide(v12, v23, v31, depth - 1);
}

void generateSubdividedIcosahedron() {
    triangles.clear();
    for (int i = 0; i < 20; i++) {
        Vector3 v1(vdata[tindices[i][0]][0], vdata[tindices[i][0]][1], vdata[tindices[i][0]][2]);
        Vector3 v2(vdata[tindices[i][1]][0], vdata[tindices[i][1]][1], vdata[tindices[i][1]][2]);
        Vector3 v3(vdata[tindices[i][2]][0], vdata[tindices[i][2]][1], vdata[tindices[i][2]][2]);
        subdivide(v1, v2, v3, subdivision_depth);
    }
}

void drawFlatIcosahedron() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        float normal[3];
        calculateNormal(i, normal);
        glNormal3fv(normal);
        for (int j = 0; j < 3; j++) {
            glVertex3fv(vdata[tindices[i][j]]);
        }
    }
    glEnd();
}

void drawInterpolateIcosahedron() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 3; j++) {
            glNormal3fv(vdata[tindices[i][j]]); // 頂點位置即為球面法向量
            glVertex3fv(vdata[tindices[i][j]]);
        }
    }
    glEnd();
}

void drawSubdividedIcosahedron() {
    glBegin(GL_TRIANGLES);
    for (const auto& tri : triangles) {
        glNormal3f(tri.v1.x, tri.v1.y, tri.v1.z);
        glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);
        
        glNormal3f(tri.v2.x, tri.v2.y, tri.v2.z);
        glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);
        
        glNormal3f(tri.v3.x, tri.v3.y, tri.v3.z);
        glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
    }
    glEnd();
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setupMaterial() {
    GLfloat mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.6f, 0.4f, 1.0f};
    GLfloat mat_ambient[] = {0.8f, 0.5f, 0.4f, 1.0f};
    GLfloat mat_shininess = 20.0f;
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glShadeModel(GL_SMOOTH);
}

void setupViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void drawScene(int mode) {
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -3.0f);
    glRotatef(rotation_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation_y, 0.0f, 1.0f, 0.0f);
    
    if (polygon_fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    
    switch (mode) {
        case 0:
            drawFlatIcosahedron();
            break;
        case 1:
            drawInterpolateIcosahedron();
            break;
        case 2:
            drawSubdividedIcosahedron();
            break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int viewport_width = window_width / 3;
    int viewport_height = window_height;
    setupViewport(0, 0, viewport_width, viewport_height);
    drawScene(0);
    setupViewport(viewport_width, 0, viewport_width, viewport_height);
    drawScene(1);
    setupViewport(viewport_width * 2, 0, viewport_width, viewport_height);
    drawScene(2);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+':
        case '=':
            if (subdivision_depth < 4) {
                subdivision_depth++;
                generateSubdividedIcosahedron();
            }
            break;
        case '-':
            if (subdivision_depth > 0) {
                subdivision_depth--;
                if (subdivision_depth > 0) generateSubdividedIcosahedron();
            }
            break;
        case 'l':
        case 'L':
            polygon_fill = false;
            break;
        case 'f':
        case 'F':
            polygon_fill = true;
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouse_left_down = true;
            mouse_x = x;
            mouse_y = y;
        } else {
            mouse_left_down = false;
        }
    }
}

void motion(int x, int y) {
    if (mouse_left_down) {
        rotation_y += (x - mouse_x) * 0.5f;
        rotation_x += (y - mouse_y) * 0.5f;
        mouse_x = x;
        mouse_y = y;
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    window_width = w;
    window_height = h;
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    setupLighting();
    setupMaterial();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("113598017 - LAB12");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}