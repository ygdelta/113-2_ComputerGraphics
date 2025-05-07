#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;

// 視窗大小
int windowWidth = 800;
int windowHeight = 600;

// 全域變數
GLfloat axis[3] = { 0.0f, 0.0f, 0.0f };
// 每個viewport獨立的旋轉軸
GLfloat v1[2][3] = { { 5.0f, 10.0f, -5.0f }, { 5.0f, 10.0f, -5.0f } };
GLfloat v2[2][3] = { { -5.0f, -10.0f, 5.0f }, { -5.0f, -10.0f, 5.0f } };

// 是否已經點擊了鼠標
bool isMouseClicked[2] = { false, false };
// 點擊的坐標點
GLfloat clickedPoint[2][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

// 立方體頂點
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

// 立方體顏色
GLfloat colors[][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f}
};

// 活動視窗的背景顏色
GLfloat activeViewportBackground[3] = { 0.2f, 0.2f, 0.2f };
GLfloat inactiveViewportBackground[3] = { 0.0f, 0.0f, 0.0f };

// 當前活動的viewport (0 表示左邊, 1 表示右邊)
int activeViewport = 0;

// 每個視窗的變換參數
GLfloat angle[2] = { 0.0f, 0.0f };
GLfloat x_prefix[2] = { 0.0f, 0.0f };
GLfloat y_prefix[2] = { 0.0f, 0.0f };
GLfloat z_prefix[2] = { 0.0f, 0.0f };
GLfloat x_scale[2] = { 1.0f, 1.0f };
GLfloat y_scale[2] = { 1.0f, 1.0f };
GLfloat z_scale[2] = { 1.0f, 1.0f };

// 函數聲明
void Scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);
void Rotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void Translate(GLfloat tx, GLfloat ty, GLfloat tz);
void RenderScene();
void RenderCube();
void ChangeSize(int, int);
void keyBoard(unsigned char, int, int);
void RenderAxis();
void RenderViewport(int viewportID);
void mouse(int button, int state, int x, int y);
void renderBitmapString(float x, float y, void* font, const char* string, float r, float g, float b);
void RenderClickMarker(int viewportID);

// 將屏幕坐標轉換為世界坐標
void ScreenToWorld(int x, int y, int viewportID, GLfloat* worldX, GLfloat* worldY, GLfloat* worldZ) {
    // 簡化的版本，將鼠標點擊映射到一個平面
    // 這個平面在z=0且與相機面對的方向平行

    // 計算點擊位置佔視窗的比例
    float xRatio = (float)x / (windowWidth / 2);
    float yRatio = (float)(windowHeight - y) / windowHeight; // 翻轉Y軸

    // 將比例映射到世界坐標系範圍
    // 在我們的視角下，視野大約在-10到10之間
    *worldX = (xRatio - 0.5f) * 20.0f;
    *worldY = (yRatio - 0.5f) * 20.0f;
    *worldZ = 0.0f; // 假設點擊在z=0平面上
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("LAB 5");

    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(keyBoard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}

// 鍵盤處理函數
void keyBoard(unsigned char key, int x, int y) {
    switch (key) {
        // 切換當前視窗
    case 'v':
        activeViewport = (activeViewport + 1) % 2;
        break;

        // 旋轉控制
    case 'q':
        angle[activeViewport] -= 5.0f;
        if (angle[activeViewport] < 0.0f) angle[activeViewport] = 360.0f;
        break;
    case 'e':
        angle[activeViewport] += 5;
        if (angle[activeViewport] > 360.0f) angle[activeViewport] = 0.0f;
        break;

        // 平移控制
    case 'd':
        x_prefix[activeViewport] += 0.5f;
        break;
    case 'a':
        x_prefix[activeViewport] -= 0.5f;
        break;
    case ' ':
        y_prefix[activeViewport] += 0.5f;
        break;
    case 'c':
        y_prefix[activeViewport] -= 0.5f;
        break;
    case 'w':
        z_prefix[activeViewport] += 0.5f;
        break;
    case 's':
        z_prefix[activeViewport] -= 0.5f;
        break;

        // 縮放控制
    case '1':
        x_scale[activeViewport] += 0.5f;
        break;
    case '2':
        y_scale[activeViewport] += 0.5f;
        break;
    case '3':
        z_scale[activeViewport] += 0.5f;
        break;
    case '4':
        x_scale[activeViewport] -= 0.5f;
        break;
    case '5':
        y_scale[activeViewport] -= 0.5f;
        break;
    case '6':
        z_scale[activeViewport] -= 0.5f;
        break;

        // 重置
    case 'r':
        angle[activeViewport] = 0.0f;
        x_prefix[activeViewport] = 0.0f;
        y_prefix[activeViewport] = 0.0f;
        z_prefix[activeViewport] = 0.0f;
        x_scale[activeViewport] = 1.0f;
        y_scale[activeViewport] = 1.0f;
        z_scale[activeViewport] = 1.0f;
        isMouseClicked[activeViewport] = false;
        break;
    }
    glutPostRedisplay();
}

// 視窗大小變更處理
void ChangeSize(int w, int h) {
    printf("Window Size = %d X %d\n", w, h);
    windowWidth = w;
    windowHeight = h;
    glutPostRedisplay();
}

// 鼠標點擊處理
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 判斷點擊在哪個viewport
        int clickedViewport;
        int viewportX = x;

        if (x < windowWidth / 2) {
            clickedViewport = 0; // 左側viewport
        }
        else {
            clickedViewport = 1; // 右側viewport
            viewportX = x - windowWidth / 2; // 調整X座標
        }

        activeViewport = clickedViewport;

        // 將屏幕坐標轉換為世界坐標
        GLfloat worldX, worldY, worldZ;
        ScreenToWorld(viewportX, y, clickedViewport, &worldX, &worldY, &worldZ);

        // 儲存點擊位置
        clickedPoint[clickedViewport][0] = worldX;
        clickedPoint[clickedViewport][1] = worldY;
        clickedPoint[clickedViewport][2] = worldZ;

        // 設置任意旋轉軸（從原點到點擊點）
        // 點擊點作為旋轉軸的一端
        v1[clickedViewport][0] = clickedPoint[clickedViewport][0];
        v1[clickedViewport][1] = clickedPoint[clickedViewport][1];
        v1[clickedViewport][2] = clickedPoint[clickedViewport][2];

        // 原點作為旋轉軸的另一端
        v2[clickedViewport][0] = 0.0f;
        v2[clickedViewport][1] = 0.0f;
        v2[clickedViewport][2] = 0.0f;

        isMouseClicked[clickedViewport] = true;

        // 輸出點擊位置
        printf("Clicked at (%f, %f, %f) in viewport %d\n",
            clickedPoint[clickedViewport][0],
            clickedPoint[clickedViewport][1],
            clickedPoint[clickedViewport][2],
            clickedViewport);

        // 立即重繪
        glutPostRedisplay();
    }
}

// 渲染文字
void renderBitmapString(float x, float y, void* font, const char* string, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

// 額外的函數，用於在點擊後繪製紅點標記（渲染在所有其他物體之上）
void RenderClickMarker(int viewportID) {
    if (isMouseClicked[viewportID]) {
        // 禁用深度測試，確保紅點始終可見
        glDisable(GL_DEPTH_TEST);

        // 設置正交投影以精確定位
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPerspective(60.0f, (GLfloat)(windowWidth / 2) / (GLfloat)windowHeight, 0.1f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // 極大的點，確保可見
        glPointSize(10.0f);
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);  // 純紅色
        glVertex3fv(clickedPoint[viewportID]);
        glEnd();

        // 恢復矩陣和狀態
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // 重新啟用深度測試
        glEnable(GL_DEPTH_TEST);
    }
}

// 渲染整個場景
void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 渲染左側視窗
    RenderViewport(0);

    // 渲染右側視窗
    RenderViewport(1);

    glutSwapBuffers();
}

// 渲染單一視窗
void RenderViewport(int viewportID) {
    // 設置視窗
    if (viewportID == 0) {
        glViewport(0, 0, windowWidth / 2, windowHeight);
    }
    else {
        glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);
    }

    // 設置背景顏色
    /*if (viewportID == activeViewport) {
        glClearColor(activeViewportBackground[0], activeViewportBackground[1], activeViewportBackground[2], 1.0);
    }
    else {
        glClearColor(inactiveViewportBackground[0], inactiveViewportBackground[1], inactiveViewportBackground[2], 1.0);
    }*/

    // 只清除當前視窗區域
    glScissor(viewportID * windowWidth / 2, 0, windowWidth / 2, windowHeight);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // 設置透視投影
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLfloat)(windowWidth / 2) / (GLfloat)windowHeight, 0.1f, 100.0f);

    // 設置視點
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // 繪製坐標軸
    RenderAxis();

    // 如果已經點擊了鼠標，則渲染點擊點和任意旋轉軸
    if (isMouseClicked[viewportID]) {
        // 暫時禁用深度測試，確保點能被看到
        glDisable(GL_DEPTH_TEST);

        // 渲染點擊點（用更大更亮的紅色點顯示）
        glPointSize(15.0f);  // 更大的點
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);  // 紅色點
        glVertex3fv(clickedPoint[viewportID]);
        glEnd();

        // 渲染原點到點擊點的旋轉軸（而不是無限延伸）
        glLineWidth(3.0f);  // 更粗的線
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f);  // 黃色線更明顯
        glVertex3f(0.0f, 0.0f, 0.0f);  // 從原點開始
        glVertex3fv(clickedPoint[viewportID]);  // 到點擊點結束
        glEnd();
        glLineWidth(1.0f);  // 恢復線寬

        // 重新啟用深度測試
        glEnable(GL_DEPTH_TEST);
    }

    // 應用旋轉、平移和縮放（使用當前viewport的旋轉軸）
    Rotation(angle[viewportID],
        v1[viewportID][0] - v2[viewportID][0],
        v1[viewportID][1] - v2[viewportID][1],
        v1[viewportID][2] - v2[viewportID][2]);
    Translate(x_prefix[viewportID], y_prefix[viewportID], z_prefix[viewportID]);
    Scale(x_scale[viewportID], y_scale[viewportID], z_scale[viewportID]);

    // 渲染立方體
    RenderCube();

    // 在最上層渲染紅點標記
    RenderClickMarker(viewportID);

    // 渲染視窗標籤
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth / 2, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 顯示當前視窗狀態
    char viewportLabel[50];

    if (viewportID == activeViewport) {
        sprintf(viewportLabel, "Viewport %d (Active)", viewportID + 1);
        renderBitmapString(10, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, viewportLabel, 1.0f, 1.0f, 0.0f);
    }
    else {
        sprintf(viewportLabel, "Viewport %d", viewportID + 1);
        renderBitmapString(10, windowHeight - 20, GLUT_BITMAP_HELVETICA_18, viewportLabel, 0.7f, 0.7f, 0.7f);
    }
}

// 渲染坐標軸
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

// 縮放矩陣實現
void Scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
    GLfloat scaleMatrix[16] = {
        scaleX,   0.0f,   0.0f, 0.0f,
        0.0f  , scaleY,   0.0f, 0.0f,
        0.0f  ,   0.0f, scaleZ, 0.0f,
        0.0f  ,   0.0f,   0.0f, 1.0f
    };
    glMultMatrixf(scaleMatrix);
}

// 旋轉矩陣實現 (任意軸)
void Rotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    // 將角度轉換為弧度
    GLfloat radians = angle * 3.14159f / 180.0f;
    GLfloat cos = std::cos(radians);
    GLfloat sin = std::sin(radians);
    GLfloat c = 1.0f - cos;

    // 標準化旋轉軸
    GLfloat len = sqrt(x * x + y * y + z * z);
    if (len < 0.0001f) {
        // 避免除以零
        return;
    }
    x = x / len;
    y = y / len;
    z = z / len;

    // 旋轉矩陣
    GLfloat rotationMatrix[16] = {
        x * x * c + cos    , x * y * c - z * sin, x * z * c + y * sin, 0.0f,
        x * y * c + z * sin, y * y * c + cos    , y * z * c - x * sin, 0.0f,
        x * z * c - y * sin, y * z * c + x * sin, z * z * c + cos    , 0.0f,
        0.0f               , 0.0f               , 0.0f               , 1.0f
    };

    glMultMatrixf(rotationMatrix);
}

// 平移矩陣實現
void Translate(GLfloat tx, GLfloat ty, GLfloat tz) {
    GLfloat transMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        tx  , ty  , tz  , 1.0f
    };
    glMultMatrixf(transMatrix);
}

// 渲染立方體
void RenderCube() {
    // 前面 (紅色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[0]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[5]);
    glEnd();

    // 左面 (綠色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[1]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[4]);
    glEnd();

    // 右面 (藍色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[2]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[1]);
    glEnd();

    // 後面 (黃色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[3]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[0]);
    glEnd();

    // 底面 (青色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[4]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glEnd();

    // 頂面 (紫色)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(colors[5]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();
}