#pragma once

struct Vertex
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double normal[3];
	double shininess;
};

typedef struct _Light
{
	double position[3];
	double color[3];
} Light;

typedef struct _Triangle
{
	struct Vertex v[3];
	void AddLight(Light light, Vector3& color, Vector3& pos);
} Triangle;

typedef struct _Sphere
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double shininess;
	double radius;

	void AddLight(Light light, Vector3& color, Vector3& pos);
} Sphere;

void Sphere::AddLight(Light light, Vector3& color, Vector3& pos)
{
	Vector3 center = Vector3(position[0], position[1], position[2]);
	Vector3 normal = pos - center;
	Vector3 lightLoc = Vector3(light.position[0], light.position[1], light.position[2]);
	Vector3 lightNormal = lightLoc - pos;
	Normalize(lightNormal);
	Normalize(lightLoc);
	double dotScale = 2 * Dot(lightLoc - pos, normal);
	Vector3 n = normal;
	Normalize(normal);
	Normalize(n);
	Multiply(n, dotScale);
	Vector3 l = lightLoc;
	Multiply(l, 2);
	Vector3 reflection = n - l;
	Normalize(reflection);
	Vector3 cam = pos;
	Multiply(cam, -1);
	Normalize(cam);
	color.x = color.x * (color_diffuse[0] * Dot(lightNormal, normal) + color_specular[0] * pow(Dot(reflection, cam), shininess));
	color.y = color.y * (color_diffuse[1] * Dot(lightNormal, normal) + color_specular[1] * pow(Dot(reflection, cam), shininess));
	color.z = color.z * (color_diffuse[2] * Dot(lightNormal, normal) + color_specular[2] * pow(Dot(reflection, cam), shininess));

}

void Triangle::AddLight(Light light, Vector3& color, Vector3& pos)
{
	Vector3 center;// = Vector3(position[0], position[1], position[2]);
	Vector3 normal = pos - center;
	Vector3 lightLoc = Vector3(light.position[0], light.position[1], light.position[2]);
	Normalize(lightLoc);
	double dotScale = 2 * Dot(lightLoc - pos, normal);
	Vector3 n = normal;
	Normalize(n);
	Multiply(n, dotScale);
	Vector3 l = lightLoc;
	Multiply(l, 2);
	Vector3 reflection = n - l;
	Normalize(reflection);
	Vector3 cam = pos;
	Multiply(cam, -1);
	Normalize(cam);
	color.x = color.x * (light.color[0] * Dot(lightLoc - pos, normal) + light.color[0] * pow(Dot(reflection, cam), 0.5));
	color.y = color.y * (light.color[1] * Dot(lightLoc - pos, normal) + light.color[1] * pow(Dot(reflection, cam), 0.5));
	color.z = color.z * (light.color[2] * Dot(lightLoc - pos, normal) + light.color[2] * pow(Dot(reflection, cam), 0.5));

}