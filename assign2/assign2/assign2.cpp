// assign2.cpp : Defines the entry point for the console application.
//

/*
	CSCI 480 Computer Graphics
	Assignment 2: Simulating a Roller Coaster
	C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Spline.h"
#include "Point.h"

const float Pi = 3.14159265358979;
const float ToRadians = 2 * Pi / 360.0f;

/* state of the world */
float g_vLandRotate[3] = { 0.0, 0.0, 0.0 };
float g_vLandTranslate[3] = { 0.0, 0.0, 0.0 };
float g_vLandScale[3] = { 1.0, 1.0, 1.0 };

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

Pic* g_pFloorTexture;
Pic* g_pSkyTexture;

GLuint g_iLineList; 
GLuint g_iFloorList;
GLuint g_iSkyList;

float g_fMaxDistance = 4.0f;
float g_s = 0.5;
float g_mBasisMatrix[16] = {-g_s   , 2 - g_s, g_s - 2    , g_s ,
							2 * g_s, g_s - 3, 3 - 2 * g_s, -g_s,
							-g_s   , 0      , g_s        , 0   , 
							0      , 1      , 0          , 0   };
float g_mControlMatrix[12] = {0, 0, 0,
							  0, 0, 0,
							  0, 0, 0,
							  0, 0, 0};

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
CONTROLSTATE g_ControlState = ROTATE;

/* spline struct which contains how many control points, and an array of control points */

/* the spline array */
struct Spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;


int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf ("can't open file\n");
		exit(1);
	}
  
	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct Spline *)malloc(g_iNumOfSplines * sizeof(struct Spline));

	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf ("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct Point *)malloc(iLength * sizeof(struct Point));
		g_Splines[j].numControlPoints = iLength;

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf", 
			&g_Splines[j].points[i].x, 
			&g_Splines[j].points[i].y, 
			&g_Splines[j].points[i].z) != EOF) {
			i++;
		}
	}

	free(cName);

	return 0;
}

void makeLines() 
{
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < g_iNumOfSplines; i++) {
		for (int j = 1; j < g_Splines[i].numControlPoints - 1; j++) {
			std::vector<Point> pList;
			int index = 1;
			float u = 0.5f;
			pList.push_back(g_Splines[i].points[j]);
			createControlMatrix(j, g_Splines[i], g_mControlMatrix);
			interpolatePoints(g_Splines[i].points[j], g_Splines[i].points[j + 1],
				pList, g_mBasisMatrix, g_mControlMatrix, g_fMaxDistance, index, u);
			for (int k = 0; k < index; k++) {
				glVertex3d(pList[k].x, pList[k].y, pList[k].z);
			}
		}
		glVertex3d(g_Splines[i].points[g_Splines[i].numControlPoints - 2].x, g_Splines[i].points[g_Splines[i].numControlPoints - 2].y, g_Splines[i].points[g_Splines[i].numControlPoints - 2].z);
	}
	glEnd();
}

void makeFloor()
{
	for (int y = 0; y < g_pFloorTexture->ny - 1; y++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < g_pFloorTexture->nx; x++)
		{
			glColor3f(PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - (y + 1), 0) / 255.0f, PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - (y + 1), 1) / 255.0f, PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - (y + 1), 2) / 255.0f);
			glVertex3f((x / 8.0f) - g_pFloorTexture->nx / 16.0f, ((y + 1) / 8.0f) - g_pFloorTexture->ny / 16.0f, -0.5f);
			glColor3f(PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - y, 0), PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - y, 1), PIC_PIXEL(g_pFloorTexture, x, g_pFloorTexture->ny - y, 2));
			glVertex3f((x / 8.0f) - g_pFloorTexture->nx / 16.0f, (y / 8.0f) - g_pFloorTexture->ny / 16.0f, -0.5f);
		}
		glEnd();
	}
}

void makeSky()
{
	float radius = g_pSkyTexture->nx / 4.0f;
	for (int z = 0; z < radius; z++)
	//int z = 0;
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x <= g_pSkyTexture->nx; x++)
		{
			float theta = ((float)x / g_pSkyTexture->nx) * 360 * ToRadians;
			float phi = ((float)z / radius) * 90 * ToRadians;
			float layerRadius = radius * cos(phi);
			float xCoord = layerRadius * cos(theta);
			float yCoord = layerRadius * sin(theta);
			float zCoord = radius * sin(phi);
			glColor3f(PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 0), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 1), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 2));
			glVertex3f(xCoord, yCoord, zCoord);

			theta = ((float)x / g_pSkyTexture->nx) * 360 * ToRadians; 
			phi = ((float)(z + 1) / radius) * 90 * ToRadians;
			layerRadius = radius * cos(phi);
			xCoord = layerRadius * cos(theta);
			yCoord = layerRadius * sin(theta);
			zCoord = radius * sin(phi);
			glColor3f(PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 0), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 1), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 2));
			glVertex3f(xCoord, yCoord, zCoord);
		}
		glEnd();
	}
}

void myinit()
{
	g_iLineList = glGenLists(1);
	glNewList(g_iLineList, GL_COMPILE);
	makeLines();
	glEndList();

	g_iFloorList = glGenLists(2);
	glNewList(g_iFloorList, GL_COMPILE);
	makeFloor();
	glEndList();

	g_iSkyList = glGenLists(3);
	glNewList(g_iSkyList, GL_COMPILE);
	makeSky();
	glEndList();

	glClearColor(0.0, 0.0, 0.0, 0.0);   // set background color
	glEnable(GL_DEPTH_TEST);            // enable depth buffering
	glShadeModel(GL_SMOOTH);            // interpolate colors during rasterization
}

void doIdle()
{
	/* do some stuff... */

	/* make the screen update */
	glutPostRedisplay();
}

void display()
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // reset transformation
	// look at the center of the image
	gluLookAt(10.0f, -20.0f, 0.0f, 10.0f, 21.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	// apply transformations
	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0], 1, 0, 0);
	glRotatef(g_vLandRotate[1], 0, 1, 0);
	glRotatef(g_vLandRotate[2], 0, 0, 1);
	glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

	glCallList(g_iLineList);
	glCallList(g_iFloorList);
	glCallList(g_iSkyList);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glFrustum(-0.5f, 1.5f, -0.5f, 1.5f, 0.5f, 10.0f);
	gluPerspective(80.0f, (float)w / h, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mousedrag(int x, int y)
{
	int vMouseDelta[2] = { x - g_vMousePos[0], y - g_vMousePos[1] };

	switch (g_ControlState)
	{
	case TRANSLATE:
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0] * 0.01;
			g_vLandTranslate[1] -= vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1] * 0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[0] += vMouseDelta[1];
			g_vLandRotate[1] += vMouseDelta[0];
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0 + vMouseDelta[0] * 0.01;
			g_vLandScale[1] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state == GLUT_DOWN);
		break;
	}

	switch (glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}
	g_pFloorTexture = jpeg_read("Ground.jpg", NULL);
	if (!g_pFloorTexture)
	{
		printf("no ground texture file found. \n");
	}
	g_pSkyTexture = jpeg_read("Sky.jpg", NULL);
	if (!g_pFloorTexture)
	{
		printf("no sky texture file found. \n");
	}
	glutInit(&argc, (char**)argv);

	loadSplines(argv[1]);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Assignment 2");

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	/* allow the user to quit using the right mouse button menu */

	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);


	/* do initialization */
	myinit();

	glutMainLoop();

	return 0;
}