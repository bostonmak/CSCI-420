/*
CSCI 480
Assignment 3 Raytracer

Name: Boston Mak
*/

#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string>
#include <math.h>
#include <cmath>
#include <limits>

#include "Vector3.h"
#include "Geometry.h"
#include "Ray.h"

static const int MAX_TRIANGLES = 2000;
static const int MAX_SPHERES = 10;
static const int MAX_LIGHTS = 10;

char *filename = 0;

//different display modes
static const int MODE_DISPLAY = 1;
static const int MODE_JPEG = 2;
int mode = MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
static const int WIDTH = 640;
static const int HEIGHT = 480;

//the field of view of the camera
static const double fov = 60.0;

static const double PI = atan(1) * 4;

unsigned char buffer[HEIGHT][WIDTH][3];

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

void AddAmbient(Vector3& color)
{
	color.x += ambient_light[0] * 255;
	color.y += ambient_light[1] * 255;
	color.z += ambient_light[2] * 255;
	if (color.x > 255)
	{
		color.x = 255;
	}
	if (color.y > 255)
	{
		color.y = 255;
	}
	if (color.z > 255)
	{
		color.z = 255;
	}
	if (color.x < 0)
	{
		color.x = 0;
	}
	if (color.y < 0)
	{
		color.y = 0;
	}
	if (color.z < 0)
	{
		color.z = 0;
	}
}

bool AddShadow(Light light, Vector3& color, Vector3& pos, void* shape)
{
	Vector3 lightLoc = Vector3(light.position[0], light.position[1], light.position[2]);
	Vector3 lightDir = lightLoc - pos;
	double lightMagnitude = lightDir.Magnitude();
	Normalize(lightDir);
	Ray ray = Ray(pos, lightDir, lightMagnitude);
	Vector3 furthestIntersect = Vector3::Zero();
	Vector3 intersect = Vector3::Zero();
	Vector3 normal = Vector3::Zero();
	bool collision = false;
	for (int i = 0; i < num_spheres; i++)
	{
		if (ray.IntersectsSphere(spheres[i], intersect, normal) && &spheres[i] != shape)
		{
			collision = true;
		}
	}
	for (int i = 0; i < num_triangles; i++)
	{
		if (ray.IntersectsTriangle(triangles[i], intersect, normal) && &triangles[i] != shape)
		{
			collision = true;
		}
	}
	if (collision && (furthestIntersect - pos).Magnitude() > 0.5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//MODIFY THIS FUNCTION
void draw_scene()
{
	double screenWidth = (static_cast<double>(WIDTH) / HEIGHT) * tan((fov * PI / 180) / 2);
	double screenHeight = tan((fov * PI / 180) / 2);
	double pixelSpace = (2 * screenWidth) / WIDTH;
	printf("w: %f, spacew: %f, spaceh: %f\n", screenWidth, (2 * screenWidth) / WIDTH, (2 * screenHeight) / HEIGHT);
	int xPixel = 0, yPixel = 0;
	// screen width/height are negative originally
	for (double x = screenWidth * -1; x < screenWidth; x += pixelSpace)
	{
		glBegin(GL_POINTS);
		glPointSize(2.0);
		for (double y = screenHeight * -1; y < screenHeight; y += pixelSpace)
		{
			Vector3 origin = Vector3();
			Vector3 direction = Vector3(x + pixelSpace / 2, y + pixelSpace / 2, -1);
			Normalize(direction);
			Ray ray = Ray(origin, direction, 100);
			Vector3 closestIntersection = Vector3(0, 0, -100);
			for (int i = 0; i < num_triangles; i++)
			{
				Vector3 intersect = Vector3(0, 0, -100);
				Vector3 normal;
				if (ray.IntersectsTriangle(triangles[i], intersect, normal))
				{
					//printf("triangle intersected\n");
					if (intersect.z > closestIntersection.z)
					{
						closestIntersection = intersect;
					}
					Vector3 tricolor = Vector3(0, 0, 0);
					for (int j = 0; j < num_lights; j++)
					{
						if (!AddShadow(lights[j], tricolor, closestIntersection, &triangles[i]))
						{
							triangles[i].AddLight(lights[j], tricolor, closestIntersection);
						}
					}
					AddAmbient(tricolor);
					//AddLight(tricolor, closestIntersection, normal);
					plot_pixel(xPixel, yPixel, tricolor.x, tricolor.y, tricolor.z);
					break;
				}
			}
			for (int i = 0; i < num_spheres; i++)
			{
				Vector3 intersect = Vector3(0, 0, -100);
				Vector3 normal;
				if (ray.IntersectsSphere(spheres[i], intersect, normal))
				{
					if (intersect.z > closestIntersection.z)
					{
						closestIntersection = intersect;
					}
					Vector3 sphereColor = Vector3(0, 0, 0);
					for (int j = 0; j < num_lights; j++)
					{
						if (!AddShadow(lights[j], sphereColor, closestIntersection, &spheres[i]))
						{
							spheres[i].AddLight(lights[j], sphereColor, closestIntersection);
						}
						
					}
					AddAmbient(sphereColor);
					plot_pixel(xPixel, yPixel, sphereColor.x, sphereColor.y, sphereColor.z);
					break;
				}
			}
			if (closestIntersection.z == -100)
			{
				plot_pixel(xPixel, yPixel, 0, 0, 0);
			}
			yPixel++;
		}
		glEnd();
		glFlush();
		xPixel++;
		yPixel = 0;
	}
// 	unsigned int x, y;
// 	//simple output
// 	for (x = 0; x < WIDTH; x++)
// 	{
// 		glBegin(GL_POINTS);
// 		glPointSize(2.0);
// 		for (y = 0; y < HEIGHT; y++)
// 		{
// 			plot_pixel(x, y, x % 256, y % 256, (x + y) % 256);
// 		}
// 		glEnd();
// 		glFlush();
// 	}
// 	printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	glColor3f(((double)r) / 256.f, ((double)g) / 256.f, ((double)b) / 256.f);
	glVertex2i(x, y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	buffer[HEIGHT - y - 1][x][0] = r;
	buffer[HEIGHT - y - 1][x][1] = g;
	buffer[HEIGHT - y - 1][x][2] = b;
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	plot_pixel_display(x, y, r, g, b);
	if (mode == MODE_JPEG)
		plot_pixel_jpeg(x, y, r, g, b);
}

void save_jpg()
{
	Pic *in = NULL;

	in = pic_alloc(640, 480, 3, NULL);
	printf("Saving JPEG file: %s\n", filename);

	memcpy(in->pix, buffer, 3 * WIDTH*HEIGHT);
	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);

}

void parse_check(char *expected, char *found)
{
	if (stricmp(expected, found))
	{
		char error[100];
		printf("Expected '%s ' found '%s '\n", expected, found);
		printf("Parse error, abnormal abortion\n");
		exit(0);
	}

}

void parse_doubles(FILE*file, char *check, double p[3])
{
	char str[100];
	fscanf(file, "%s", str);
	parse_check(check, str);
	fscanf(file, "%lf %lf %lf", &p[0], &p[1], &p[2]);
	printf("%s %lf %lf %lf\n", check, p[0], p[1], p[2]);
}

void parse_rad(FILE*file, double *r)
{
	char str[100];
	fscanf(file, "%s", str);
	parse_check("rad:", str);
	fscanf(file, "%lf", r);
	printf("rad: %f\n", *r);
}

void parse_shi(FILE*file, double *shi)
{
	char s[100];
	fscanf(file, "%s", s);
	parse_check("shi:", s);
	fscanf(file, "%lf", shi);
	printf("shi: %f\n", *shi);
}

int loadScene(char *argv)
{
	FILE *file = fopen(argv, "r");
	int number_of_objects;
	char type[50];
	int i;
	Triangle t;
	Sphere s;
	Light l;
	fscanf(file, "%i", &number_of_objects);

	printf("number of objects: %i\n", number_of_objects);
	char str[200];

	parse_doubles(file, "amb:", ambient_light);

	for (i = 0; i < number_of_objects; i++)
	{
		fscanf(file, "%s\n", type);
		printf("%s\n", type);
		if (stricmp(type, "triangle") == 0)
		{
			printf("found triangle\n");
			int j;

			for (j = 0; j < 3; j++)
			{
				parse_doubles(file, "pos:", t.v[j].position);
				parse_doubles(file, "nor:", t.v[j].normal);
				parse_doubles(file, "dif:", t.v[j].color_diffuse);
				parse_doubles(file, "spe:", t.v[j].color_specular);
				parse_shi(file, &t.v[j].shininess);
			}

			if (num_triangles == MAX_TRIANGLES)
			{
				printf("too many triangles, you should increase MAX_TRIANGLES!\n");
				exit(0);
			}
			triangles[num_triangles++] = t;
		}
		else if (stricmp(type, "sphere") == 0)
		{
			printf("found sphere\n");

			parse_doubles(file, "pos:", s.position);
			parse_rad(file, &s.radius);
			parse_doubles(file, "dif:", s.color_diffuse);
			parse_doubles(file, "spe:", s.color_specular);
			parse_shi(file, &s.shininess);

			if (num_spheres == MAX_SPHERES)
			{
				printf("too many spheres, you should increase MAX_SPHERES!\n");
				exit(0);
			}
			spheres[num_spheres++] = s;
		}
		else if (stricmp(type, "light") == 0)
		{
			printf("found light\n");
			parse_doubles(file, "pos:", l.position);
			parse_doubles(file, "col:", l.color);

			if (num_lights == MAX_LIGHTS)
			{
				printf("too many lights, you should increase MAX_LIGHTS!\n");
				exit(0);
			}
			lights[num_lights++] = l;
		}
		else
		{
			printf("unknown type in scene description:\n%s\n", type);
			exit(0);
		}
	}
	return 0;
}

void display()
{

}

void init()
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WIDTH, 0, HEIGHT, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
	//hack to make it only draw once
	static int once = 0;
	if (!once)
	{
		draw_scene();
		if (mode == MODE_JPEG)
			save_jpg();
	}
	once = 1;
}

int main(int argc, char ** argv)
{
	if (argc < 2 || argc > 3)
	{
		printf("usage: %s <scenefile> [jpegname]\n", argv[0]);
		exit(0);
	}
	if (argc == 3)
	{
		mode = MODE_JPEG;
		filename = argv[2];
	}
	else if (argc == 2)
		mode = MODE_DISPLAY;

	glutInit(&argc, argv);
	loadScene(argv[1]);

	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	int window = glutCreateWindow("Ray Tracer");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
}
