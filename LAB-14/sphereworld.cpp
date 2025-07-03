// SphereWorld.cpp
// OpenGL SuperBible
// Modified version with animated robot
// Program by Richard S. Wright Jr.

#include <math.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <stdlib.h>
#include "glframe.h"
#include "math3d.h"

typedef float M3DVector3f[3];
typedef float M3DVector4f[4];
typedef float M3DMatrix44f[16];

// Animation control
bool isPaused = false;
float robotArmAngle = 0.0f;
float robotLegAngle = 0.0f;
float robotWalkCycle = 0.0f;


// OpenGL drawing utility functions
void gltDrawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks) {
    GLUquadricObj *pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricTexture(pObj, GL_TRUE);
    gluSphere(pObj, fRadius, iSlices, iStacks);
    gluDeleteQuadric(pObj);
}

void gltDrawCube(GLfloat size) {
    GLfloat s = size / 2.0f;
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
    
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
    
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
    
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, s);
    
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, s);
    
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
    glEnd();
}

// Simple TGA loader (simplified version)
GLbyte* gltLoadTGA(const char* szFileName, GLint* iWidth, GLint* iHeight, GLint* iComponents, GLenum* eFormat) {
    // This is a simplified placeholder - in a real implementation you'd load actual TGA files
    // For now, we'll create a simple procedural texture
    *iWidth = 512;
    *iHeight = 512;
    *iComponents = 3;
    *eFormat = GL_RGB;
    
    GLbyte* pData = (GLbyte*)malloc(512 * 512 * 3);
    
    // Create a simple checkerboard pattern based on filename
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            int index = (i * 512 + j) * 3;
            
            if (strstr(szFileName, "grass")) {
                // Green texture
                pData[index] = 50 + (i + j) % 100;      // R
                pData[index + 1] = 150 + (i + j) % 100; // G
                pData[index + 2] = 50 + (i + j) % 50;   // B
            } else if (strstr(szFileName, "wood")) {
                // Brown wood texture with grain
                int grain = (i % 32 < 16) ? 20 : 0;
                pData[index] = 139 + grain + (i % 20);     // R
                pData[index + 1] = 69 + grain + (i % 15);  // G
                pData[index + 2] = 19 + grain + (i % 10);  // B
            } else {
                // Orb texture - bluish
                pData[index] = 100 + (i + j) % 50;      // R
                pData[index + 1] = 150 + (i + j) % 50;  // G
                pData[index + 2] = 200 + (i + j) % 50;  // B
            }
        }
    }
    
    return pData;
}

#define NUM_SPHERES      2
GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;

// Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

M3DMatrix44f mShadowMatrix;

#define GROUND_TEXTURE  0
#define TORUS_TEXTURE   1
#define SPHERE_TEXTURE  2
#define NUM_TEXTURES    3
GLuint  textureObjects[NUM_TEXTURES];

const char *szTextureFiles[] = {"grass.tga", "wood.tga", "orb.tga"};

// Draw the animated robot
void DrawRobot(GLint nShadow) {
    static GLfloat yRot = 0.0f;
    
    if(nShadow == 0 && !isPaused) {
        yRot += 0.5f;
        robotWalkCycle += 0.05f;
        robotArmAngle = sin(robotWalkCycle) * 30.0f;
        robotLegAngle = sin(robotWalkCycle) * 25.0f;
    }
    
    // Set color for robot or shadow
    if(nShadow == 0) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    }
    
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, -2.5f);  // Position robot on the grass
    glScalef(0.5f, 0.5f, 0.5f);  // Make robot much smaller
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);  // Spin the robot
    
    glBindTexture(GL_TEXTURE_2D, textureObjects[TORUS_TEXTURE]);
    
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    gltDrawCube(0.6f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    gltDrawCube(0.4f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.5f, 0.5f, 0.0f);
    glRotatef(robotArmAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.2f, 0.0f, 0.0f);
    glScalef(1.5f, 0.3f, 0.3f);
    gltDrawCube(0.3f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.0f);
    glRotatef(-robotArmAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.2f, 0.0f, 0.0f);
    glScalef(1.5f, 0.3f, 0.3f);
    gltDrawCube(0.3f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.15f, 0.0f, 0.0f);
    glRotatef(robotLegAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.3f, 1.2f, 0.3f);
    gltDrawCube(0.4f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.15f, 0.0f, 0.0f);
    glRotatef(-robotLegAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.3f, 1.2f, 0.3f);
    gltDrawCube(0.4f);
    glPopMatrix();
    
    glPopMatrix();
}

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SetupRC() {
    M3DVector3f vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
                             { 10.0f, -0.4f, 0.0f },
                             { 5.0f, -0.4f, -5.0f }};
    int iSphere;
    int i;
    
    // Grayish background
    glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);
   
    // Clear stencil buffer with zero, increment by one whenever anybody
    // draws into it. When stencil function is enabled, only write where
    // stencil value is zero. This prevents the transparent shadow from drawing
    // over itself
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 0x0, 0x01);
    
    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    
    // Setup light parameters
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
        
    // Calculate shadow matrix
    M3DVector4f pPlane;
    m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
    m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fLightPos);
    
    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
  
    
    // Randomly place the sphere inhabitants
    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++) {
        // Place 2 spheres at specific locations
        if(iSphere == 0)
            spheres[iSphere].SetOrigin(-5.0f, 0.0f, -3.0f);
        else
            spheres[iSphere].SetOrigin(5.0f, 0.0f, -3.0f);
    }
      
    // Set up texture maps
    glEnable(GL_TEXTURE_2D);
    glGenTextures(NUM_TEXTURES, textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    
    for(i = 0; i < NUM_TEXTURES; i++) {
        GLbyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;
        
        glBindTexture(GL_TEXTURE_2D, textureObjects[i]);
        
        // Load this texture map
        pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

}

////////////////////////////////////////////////////////////////////////
// Do shutdown for the rendering context
void ShutdownRC(void) {
    // Delete the textures
    glDeleteTextures(NUM_TEXTURES, textureObjects);
}


///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
void DrawGround(void) {
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLint iStrip, iRun;
    GLfloat s = 0.0f;
    GLfloat t = 0.0f;
    GLfloat texStep = 1.0f / (fExtent * .075f);
    
    glBindTexture(GL_TEXTURE_2D, textureObjects[GROUND_TEXTURE]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
        {
        t = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);

            for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
                {
                glTexCoord2f(s, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip, y, iRun);
                
                glTexCoord2f(s + texStep, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip + fStep, y, iRun);
                
                t += texStep;
                }
        glEnd();
        s += texStep;
        }
}

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating robot
void DrawInhabitants(GLint nShadow) {
    GLint i;
    static GLfloat yRot = 0.0f;         // Rotation angle for animation

    if(nShadow == 0 && !isPaused) {
        yRot += 0.5f;
    }

    if(nShadow == 0) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color
    }
        
    // Draw the randomly located spheres - bind orb texture
    glBindTexture(GL_TEXTURE_2D, textureObjects[SPHERE_TEXTURE]);
    for(i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        gltDrawSphere(0.3f, 21, 11);
        glPopMatrix();
    }

    // Draw small orbiting sphere (also uses orb texture)
    glPushMatrix();
        glTranslatef(0.0f, 0.1f, -2.5f);
    
        glPushMatrix();
            glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(1.0f, 0.0f, 0.0f);
            gltDrawSphere(0.1f, 21, 11);
        glPopMatrix();
    glPopMatrix();

    // Draw the giant robot in center
    DrawRobot(nShadow);
}

        
// Called to draw scene
void RenderScene(void) {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
    glPushMatrix();
        frameCamera.ApplyCameraTransform();
        
        // Position light before any other transformations
        glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
        
        // Draw the ground
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawGround();
        
        // Draw shadows first
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);
        glPushMatrix();
            glMultMatrixf(mShadowMatrix);
            DrawInhabitants(1);
        glPopMatrix();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        
        // Draw inhabitants normally
        DrawInhabitants(0);

    glPopMatrix();
        
    // Do the buffer Swap
    glutSwapBuffers();
}

// Keyboard function to handle pause/unpause
void KeyboardFunc(unsigned char key, int x, int y) {
    if(key == 'p' || key == 'P') {
        isPaused = !isPaused;
    }
    // Refresh the Window
    glutPostRedisplay();
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.1f);

    if(key == GLUT_KEY_DOWN)
		frameCamera.MoveForward(-0.1f);

    if(key == GLUT_KEY_LEFT)
		frameCamera.RotateLocalY(0.1f);
	   
    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1f);
                        
    // Refresh the Window
    glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value) {
    // Redraw the scene with new coordinates
    glutPostRedisplay();
    glutTimerFunc(3,TimerFunction, 1);
}

void ChangeSize(int w, int h) {
    GLfloat fAspect;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    glViewport(0, 0, w, h);
        
    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800,600);
    glutCreateWindow("113598017 - LAB14 SphereWorld");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(KeyboardFunc);
    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);
    glutMainLoop();
    ShutdownRC();
    return 0;
}