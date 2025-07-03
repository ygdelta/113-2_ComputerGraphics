// Shadow.cpp
// OpenGL SuperBible
// Demonstrates simple planar shadows
// Program by Richard S. Wright Jr.

// #include "gltools.h"     
#include "GL/freeglut.h"
#include <math.h>
using namespace std;
// #include "math3d.h"

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static int currentLight = 0;

// These values need to be available globally
// Light values and coordinates
GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat	 lightPos0[] = { -75.0f, 150.0f, -50.0f, 0.0f };
GLfloat	 lightPos1[] = { 75.0f, 150.0f, -50.0f, 0.0f };
GLfloat	 lightPos2[] = { 0.0f, 150.0f, 100.0f, 0.0f };
GLfloat	 lightPos3[] = { 0.0f, 150.0f, -100.0f, 0.0f };
GLfloat* lightPos[4] = { lightPos0, lightPos1, lightPos2, lightPos3 };
GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };

typedef float M3DMatrix44f[16];	
typedef double M3DMatrix44d[16];
typedef float M3DVector3f[3];	
typedef double M3DVector3d[3];
typedef float M3DVector4f[4];
typedef double M3DVector4d[4];

void switchLight();

void m3dMakePlanarShadowMatrix(M3DMatrix44f projM, M3DVector4f plane, M3DVector3f lightPos) {
    float dx = -lightPos[0];
    float dy = -lightPos[1];
    float dz = -lightPos[2];

    projM[0] = plane[1] * dy + plane[2] * dz;
    projM[1] = -plane[0] * dy;
    projM[2] = -plane[0] * dz;
    projM[3] = 0.0f;
    projM[4] = -plane[1] * dx;
    projM[5] = plane[0] * dx + plane[2] * dz;
    projM[6] = -plane[1] * dz;
    projM[7] = 0.0f;
    projM[8] = -plane[2] * dx;
    projM[9] = -plane[2] * dy;
    projM[10] = plane[0] * dx + plane[1] * dy;
    projM[11] = 0.0f;
    projM[12] = -plane[3] * dx;
    projM[13] = -plane[3] * dy;
    projM[14] = -plane[3] * dz;
    projM[15] = plane[0] * dx + plane[1] * dy + plane[2] * dz;
}

void m3dCrossProduct(M3DVector3f result, const M3DVector3f u, const M3DVector3f v) {
	result[0] = u[1]*v[2] - v[1]*u[2];
	result[1] = -u[0]*v[2] + v[0]*u[2];
	result[2] = u[0]*v[1] - v[0]*u[1];
}

void m3dGetPlaneEquation(M3DVector4f planeEq, const M3DVector3f p1, const M3DVector3f p2, const M3DVector3f p3)
{
    M3DVector3f v1, v2;
    v1[0] = p3[0] - p1[0];
    v1[1] = p3[1] - p1[1];
    v1[2] = p3[2] - p1[2];

    v2[0] = p2[0] - p1[0];
    v2[1] = p2[1] - p1[1];
    v2[2] = p2[2] - p1[2];

    m3dCrossProduct(planeEq, v1, v2);
    float length = sqrt(planeEq[0] * planeEq[0] + planeEq[1] * planeEq[1] + planeEq[2] * planeEq[2]);
    for ( int i = 0; i < 3; i++ ) {
        if (length != 0.0f)
            planeEq[i] /= length;  // Normalize the vector
        else
            planeEq[i] = 0.0f;      // Avoid divide by zero
    }
    // Back substitute to get D
    planeEq[3] = -(planeEq[0] * p3[0] + planeEq[1] * p3[1] + planeEq[2] * p3[2]);
}

void m3dFindNormal(M3DVector3f result, const M3DVector3f point1, const M3DVector3f point2, 
							const M3DVector3f point3)
{
	M3DVector3f v1,v2;		// Temporary vectors

	// Calculate two vectors from the three points. Assumes counter clockwise
	// winding!
	v1[0] = point1[0] - point2[0];
	v1[1] = point1[1] - point2[1];
	v1[2] = point1[2] - point2[2];

	v2[0] = point2[0] - point3[0];
	v2[1] = point2[1] - point3[1];
	v2[2] = point2[2] - point3[2];

	// Take the cross product of the two vectors to get
	// the normal vector.
	m3dCrossProduct(result, v1, v2);
}

// Transformation matrix to project shadow
M3DMatrix44f shadowMat;

////////////////////////////////////////////////
// This function just specifically draws the jet
void DrawJet(int nShadow)
	{
	M3DVector3f vNormal;	// Storeage for calculated surface normal

	// Nose Cone /////////////////////////////
	// Set material color, note we only have to set to black
	// for the shadow once
	if(nShadow == 0)
        glColor3ub(255, 0, 0);
	else
        glColor3ub(88, 0, 0);


	// Nose Cone - Points straight down
    // Set material color
	glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, -1.0f, 0.0f);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 60.0f);
		glVertex3f(-15.0f, 0.0f, 30.0f);
		glVertex3f(15.0f,0.0f,30.0f);
                
	
        // Verticies for this panel
        {
        M3DVector3f vPoints[3] = {{ 15.0f, 0.0f,  30.0f},
                                        { 0.0f,  15.0f, 30.0f},
                                        { 0.0f,  0.0f,  60.0f}};

        // Calculate the normal for the plane
        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
		glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }	


        {
        M3DVector3f vPoints[3] = {{ 0.0f, 0.0f, 60.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { -15.0f, 0.0f, 30.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }


        // Body of the Plane ////////////////////////
        {
        M3DVector3f vPoints[3] = {{ -15.0f, 0.0f, 30.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { 0.0f, 0.0f, -56.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                	
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 0.0f, -56.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { 15.0f,0.0f,30.0f }};
	
        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                		
    
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(15.0f,0.0f,30.0f);
		glVertex3f(-15.0f, 0.0f, 30.0f);
		glVertex3f(0.0f, 0.0f, -56.0f);
    
        ///////////////////////////////////////////////
        // Left wing
        // Large triangle for bottom of wing
        {
        M3DVector3f vPoints[3] = {{ 0.0f,2.0f,27.0f },
                                  { -60.0f, 2.0f, -8.0f },
                                  { 60.0f, 2.0f, -8.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        	
        {
        M3DVector3f vPoints[3] = {{ 60.0f, 2.0f, -8.0f},
                                   {0.0f, 7.0f, -8.0f},
                                   {0.0f,2.0f,27.0f }};
                
        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{60.0f, 2.0f, -8.0f},
					{-60.0f, 2.0f, -8.0f},
					{0.0f,7.0f,-8.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{0.0f,2.0f,27.0f},
                                    {0.0f, 7.0f, -8.0f},
                                    {-60.0f, 2.0f, -8.0f}};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        // Tail section///////////////////////////////
        // Bottom of back fin
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-30.0f, -0.50f, -57.0f);
		glVertex3f(30.0f, -0.50f, -57.0f);
		glVertex3f(0.0f,-0.50f,-40.0f);

        {
        M3DVector3f vPoints[3] = {{ 0.0f,-0.5f,-40.0f },
                                  {30.0f, -0.5f, -57.0f},
                                {0.0f, 4.0f, -57.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 4.0f, -57.0f },
                { -30.0f, -0.5f, -57.0f },
                { 0.0f,-0.5f,-40.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }

        {
        M3DVector3f vPoints[3] = {{ 30.0f,-0.5f,-57.0f },
					{ -30.0f, -0.5f, -57.0f },
					{ 0.0f, 4.0f, -57.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{ 0.0f,0.5f,-40.0f },
					{ 3.0f, 0.5f, -57.0f },
					{ 0.0f, 25.0f, -65.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 25.0f, -65.0f },
					{ -3.0f, 0.5f, -57.0f},
					{ 0.0f,0.5f,-40.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{ 3.0f,0.5f,-57.0f },
					{ -3.0f, 0.5f, -57.0f },
					{ 0.0f, 25.0f, -65.0f }};

        m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
        glNormal3fv(vNormal);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                
        glEnd();
	}

// Called to draw scene
void RenderScene(void)
    {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the ground, we do manual shading to a darker green
    // in the background to give the illusion of depth
    glBegin(GL_QUADS);
        glColor3ub(0,32,0); //light green ground
        glVertex3f(400.0f, -150.0f, -200.0f);
        glVertex3f(-400.0f, -150.0f, -200.0f);
        glColor3ub(0,255,0);  // make it in green gradient
        glVertex3f(-400.0f, -150.0f, 200.0f);
        glVertex3f(400.0f, -150.0f, 200.0f);
    glEnd();

    // Save the matrix state and do the rotations
    glPushMatrix();

    // Draw jet at new orientation, put light in correct position
    // before rotating the jet
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos[currentLight]);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    DrawJet(0);

    // Restore original matrix state
    glPopMatrix();	


    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glPushMatrix();

    // Multiply by shadow projection matrix
    glMultMatrixf((GLfloat *)shadowMat);

    // Now rotate the jet around in the new flattend space
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Pass true to indicate drawing shadow
    DrawJet(1);	

    // Restore the projection to normal
    glPopMatrix();

    // Draw the light source
    glPushMatrix();
    glTranslatef(lightPos[currentLight][0], 
                 lightPos[currentLight][1], 
                 lightPos[currentLight][2]);
    glColor3ub(255,255,0);
    glutSolidSphere(5.0f,10,10);
    glPopMatrix();

    // Restore lighting state variables
    glEnable(GL_DEPTH_TEST);

    // Display the results
    glutSwapBuffers();
    }

// This function does any needed initialization on the rendering
// context. 
void SetupRC()
    {
    // Any three points on the ground (counter clockwise order)
    M3DVector3f points[3] = {{ -30.0f, -149.0f, -20.0f },
                            { -30.0f, -149.0f, 20.0f },
                            { 40.0f, -149.0f, 20.0f }};

    glEnable(GL_DEPTH_TEST);	// Hidden surface removal
    glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
    glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

    // Setup and enable light
    switchLight();
    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);
	
    // Set Material properties to follow glColor values
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // All materials hereafter have full specular reflectivity
    // with a high shine
    glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
    glMateriali(GL_FRONT,GL_SHININESS,128);

    // Light blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

    // Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);

    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos[currentLight]);
    
    glEnable(GL_NORMALIZE);
    }

void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        xRot-= 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot += 5.0f;

    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;
    if(key > 356.0f)
        xRot = 0.0f;

    if(key < -1.0f)
        xRot = 355.0f;

    if(key > 356.0f)
        yRot = 0.0f;

    if(key < -1.0f)
        yRot = 355.0f;

    // Refresh the Window
    glutPostRedisplay();
    }


void ChangeSize(int w, int h)
    {
    GLfloat fAspect;

    // Prevent a divide by zero
    if(h == 0)
        h = 1;

	
    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    // Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    fAspect = (GLfloat)w/(GLfloat)h;
    gluPerspective(60.0f, fAspect, 200.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move out Z axis so we can see everything
    glTranslatef(0.0f, 0.0f, -400.0f);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos[currentLight]);
    }

void switchLight() {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    
    GLenum lightID = GL_LIGHT0 + currentLight;
    glEnable(lightID);
    
    glLightfv(lightID, GL_AMBIENT, ambientLight);
    glLightfv(lightID, GL_DIFFUSE, diffuseLight);
    glLightfv(lightID, GL_SPECULAR, specular);
    glLightfv(lightID, GL_POSITION, lightPos[currentLight]);

    M3DVector3f points[3] = {{ -30.0f, -149.0f, -20.0f },
                            { -30.0f, -149.0f, 20.0f },
                            { 40.0f, -149.0f, 20.0f }};
    
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);
    m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos[currentLight]);
}

void KeyBoardFunc(unsigned char key, int x, int y) {
    switch ( key ) {
    case 'r':
    case 'R':
        xRot = 0.0f;
        yRot = 0.0f;
        break;
    case '1':
        currentLight = 0;
        switchLight();
        break;
    case '2':
        currentLight = 1;
        switchLight();
        break;
    case '3':
        currentLight = 2;
        switchLight();
        break;
    case '4':
        currentLight = 3;
        switchLight();
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Shadow (113598017 - LAB13)");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(KeyBoardFunc);
    SetupRC();
    glutMainLoop();

    return 0;
    }
