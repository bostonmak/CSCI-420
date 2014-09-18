// assign1.cpp : Defines the entry point for the console application.
//

/*
  CSCI 420 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
typedef enum { Fill, Lines, Points, OverlayLines, Texture} RENDERSTATE;

RENDERSTATE g_RenderState = Fill;
CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;
Pic * g_pTextureData;

GLuint g_iTriList;
GLuint g_iLineList;
GLuint g_iTextureList;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void makeTriangles(bool textured)
{
	if (textured)
	{
		for (int y = 0; y < g_pHeightData->ny - 1; y++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for (int x = 0; x < g_pHeightData->nx; x++)
			{
				glColor3f(PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - (y + 1), 0) / 255.0f, PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - (y + 1), 1) / 255.0f, PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - (y + 1), 2) / 255.0f);
				glVertex3f(x / (float)g_pHeightData->nx, (y + 1) / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y + 1, 0) / -255.0f);
				glColor3f(PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - y, 0), PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - y, 1), PIC_PIXEL(g_pTextureData, x, g_pTextureData->ny - y, 2));
				glVertex3f(x / (float)g_pHeightData->nx, y / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y, 0) / -255.0f);
			}
			glEnd();
		}
	}
	else
	{
		// draw triangle strips with a red->white gradient
		for (int y = 0; y < g_pHeightData->ny - 1; y++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for (int x = 0; x < g_pHeightData->nx; x++)
			{
				glColor3f(1.0, PIC_PIXEL(g_pHeightData, x, y + 1, 0) / 255.0f, PIC_PIXEL(g_pHeightData, x, y + 1, 0) / 255.0f);
				glVertex3f(x / (float)g_pHeightData->nx, (y + 1) / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y + 1, 0) / -255.0f);
				glColor3f(1.0, PIC_PIXEL(g_pHeightData, x, y, 0) / 255.0f, PIC_PIXEL(g_pHeightData, x, y, 0) / 255.0f);
				glVertex3f(x / (float)g_pHeightData->nx, y / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y, 0) / -255.0f);
			}
			glEnd();
		}
	}
	
}

void makeTriangles2()
{
	// draw triangle strips with semi-transparent blue for wireframe overlay
	for (int y = 0; y < g_pHeightData->ny - 1; y++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
		for (int x = 0; x < g_pHeightData->nx; x++)
		{
			glVertex3f(x / (float)g_pHeightData->nx, (y + 1) / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y + 1, 0) / -255.0f);
			glVertex3f(x / (float)g_pHeightData->nx, y / (float)g_pHeightData->ny, PIC_PIXEL(g_pHeightData, x, y, 0) / -255.0f);
		}
		glEnd();
	}
}

void drawScreen()
{
	// determine the type of rendering
	switch (g_RenderState)
	{
	case Fill:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCallList(g_iTriList);
		break;
	case Lines:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glCallList(g_iTriList);
		break;
	case Points:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glCallList(g_iTriList);
		break;
	case OverlayLines:
		// draw wireframe on top of solid gradient
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		glCallList(g_iTriList);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glCallList(g_iLineList);
		break;
	case Texture:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCallList(g_iTextureList);
		break;
	default:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCallList(g_iTriList);
		break;
	}
}

void myinit()
{
	// set up draw lists
	g_iTriList = glGenLists(1);
	glNewList(g_iTriList, GL_COMPILE);
	makeTriangles(false);
	glEndList();

	g_iLineList = glGenLists(2);
	glNewList(g_iLineList, GL_COMPILE);
	makeTriangles2();
	glEndList();

	g_iTextureList = glGenLists(3);
	glNewList(g_iTextureList, GL_COMPILE);
	if (g_pTextureData)
	{
		makeTriangles(true);
	}
	else
	{
		makeTriangles(false);
	}
	glEndList();

	glClearColor(0.0, 0.0, 0.0, 0.0);   // set background color
	glEnable(GL_DEPTH_TEST);            // enable depth buffering
	glShadeModel(GL_SMOOTH);            // interpolate colors during rasterization
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// enable blending for transparency
	glEnable(GL_BLEND);
}


void display()
{
  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // reset transformation
  // look at the center of the image
  gluLookAt(0.5f, 0.5f, 1.0f, 0.5f, 0.5f, -1.0f, 0.0f, 1.0f, 0.0f);
  // apply transformations
  glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
  glRotatef(g_vLandRotate[0], 1, 0, 0);
  glRotatef(g_vLandRotate[1], 0, 1, 0);
  glRotatef(g_vLandRotate[2], 0, 0, 1);
  glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

  // determine render mode
  drawScreen();

  glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glFrustum(-0.5f, 1.5f, -0.5f, 1.5f, 0.5f, 10.0f);
	gluPerspective(60.0f, w / h, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
  glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
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
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
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
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
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

void keypress(unsigned char key, int x, int y)
{
	// map render modes to keys 1, 2, 3, 4
	switch (key)
	{
	case ('1') :
		g_RenderState = Fill;
		break;
	case ('2') :
		g_RenderState = Lines;
		break;
	case('3') :
		g_RenderState = Points;
		break;
	case('4') :
		g_RenderState = OverlayLines;
		break;
	case('5') :
		g_RenderState = Texture;
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
//		exit(1);
	}

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", argv[1]);
	    exit(1);
	}
	g_pTextureData = jpeg_read((char*)argv[2], NULL);
	if (!g_pTextureData)
	{
		printf("no texture file found. \n");
	}

	glutInit(&argc,(char**)argv);
  
	/*
		create a window here..should be double buffered and use depth testing
  
	    the code past here will segfault if you don't have a window set up....
	    replace the exit once you add those calls.
	*/

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Assignment 1");
	
	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  
	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
  
	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	/* callback for keyboard presses */
	glutKeyboardFunc(keypress);

	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;
}