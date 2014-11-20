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
	color.x = (color_diffuse[0] * 255.0f * Dot(lightNormal, normal) + color_specular[0] * 255.0f * pow(Dot(reflection, cam), shininess));
	color.y = (color_diffuse[1] * 255.0f * Dot(lightNormal, normal) + color_specular[1] * 255.0f * pow(Dot(reflection, cam), shininess));
	color.z = (color_diffuse[2] * 255.0f * Dot(lightNormal, normal) + color_specular[2] * 255.0f * pow(Dot(reflection, cam), shininess));

}

void Triangle::AddLight(Light light, Vector3& color, Vector3& pos)
{
	// Pos = Vertex D
	Vector3 vertexA = Vector3(v[0].position[0], v[0].position[1], v[0].position[2]);
	Vector3 vertexB = Vector3(v[1].position[0], v[1].position[1], v[1].position[2]);
	Vector3 vertexC = Vector3(v[2].position[0], v[2].position[1], v[2].position[2]);

	Vector3 AB = vertexB - vertexA;
	Vector3 AD = pos - vertexA;
	Vector3 crossA;
	CrossProduct(AB, AD, crossA);
	Vector3 normalA = Vector3(v[0].normal[0], v[0].normal[1], v[0].normal[2]);
	Normalize(normalA);

	Vector3 BC = vertexC - vertexB;
	Vector3 BD = pos - vertexB;
	Vector3 crossB;
	CrossProduct(BC, BD, crossB);
	Vector3 normalB = Vector3(v[1].normal[0], v[1].normal[1], v[1].normal[2]);
	Normalize(normalB);

	Vector3 CA = vertexA - vertexC;
	Vector3 CD = pos - vertexC;
	Vector3 crossC;
	CrossProduct(CA, CD, crossC);
	Vector3 normalC = Vector3(v[2].normal[0], v[2].normal[1], v[2].normal[2]);
	Normalize(normalC);

	float areaA = crossA.Magnitude() * 0.5f;
	float areaB = crossB.Magnitude() * 0.5f;
	float areaC = crossC.Magnitude() * 0.5f;
	float totalArea = areaA + areaB + areaC;

	float ratioA = areaA / totalArea;
	float ratioB = areaB / totalArea;
	float ratioC = areaC / totalArea;

	Vector3 lightLoc = Vector3(light.position[0], light.position[1], light.position[2]);
	Vector3 lightNormal = lightLoc - pos;
	Normalize(lightNormal);
	Vector3 lightA = lightLoc - vertexA;
	Normalize(lightA);
	Vector3 lightB = lightLoc - vertexB;
	Normalize(lightB);
	Vector3 lightC = lightLoc - vertexC;
	Normalize(lightC);

	double dotScaleA = 2 * Dot(lightLoc - vertexA, normalA);
	double dotScaleB = 2 * Dot(lightLoc - vertexB, normalB);
	double dotScaleC = 2 * Dot(lightLoc - vertexC, normalC);

	Vector3 nA = normalA;
	Vector3 nB = normalB;
	Vector3 nC = normalC;
	Vector3 lA = lightA;
	Vector3 lB = lightB;
	Vector3 lC = lightC;

	Multiply(nA, dotScaleA);
	Multiply(nB, dotScaleB);
	Multiply(nC, dotScaleC);
	Multiply(lA, 2);
	Multiply(lB, 2);
	Multiply(lC, 2);

	Vector3 reflectionA = nA - lA;
	Vector3 reflectionB = nB - lB;
	Vector3 reflectionC = nC - lC;

	Normalize(reflectionA);
	Normalize(reflectionB);
	Normalize(reflectionC);

	Vector3 camA = vertexA;
	Vector3 camB = vertexB;
	Vector3 camC = vertexC;

	Multiply(camA, -1);
	Multiply(camB, -1);
	Multiply(camC, -1);

	Normalize(camA);
	Normalize(camB);
	Normalize(camC);

	Vector3 colorA, colorB, colorC;

	colorA.x = v[0].color_diffuse[0]  * 255.0f * Dot(lightA, normalA) + v[0].color_specular[0]  * 255.0f * pow(Dot(reflectionA, camA), v[0].shininess);
	colorA.y = v[0].color_diffuse[1]  * 255.0f * Dot(lightA, normalA) + v[0].color_specular[1]  * 255.0f * pow(Dot(reflectionA, camA), v[0].shininess);
	colorA.z = v[0].color_diffuse[2]  * 255.0f * Dot(lightA, normalA) + v[0].color_specular[2]  * 255.0f * pow(Dot(reflectionA, camA), v[0].shininess);
	Multiply(colorA, ratioA);	

	colorB.x = v[1].color_diffuse[0]  * 255.0f * Dot(lightA, normalA) + v[1].color_specular[0]  * 255.0f * pow(Dot(reflectionA, camA), v[1].shininess);
	colorB.y = v[1].color_diffuse[1]  * 255.0f * Dot(lightA, normalA) + v[1].color_specular[1]  * 255.0f * pow(Dot(reflectionA, camA), v[1].shininess);
	colorB.z = v[1].color_diffuse[2]  * 255.0f * Dot(lightA, normalA) + v[1].color_specular[2]  * 255.0f * pow(Dot(reflectionA, camA), v[1].shininess);
	Multiply(colorB, ratioB);	

	colorC.x = v[2].color_diffuse[0]  * 255.0f * Dot(lightA, normalA) + v[2].color_specular[0]  * 255.0f * pow(Dot(reflectionA, camA), v[2].shininess);
	colorC.y = v[2].color_diffuse[1]  * 255.0f * Dot(lightA, normalA) + v[2].color_specular[1]  * 255.0f * pow(Dot(reflectionA, camA), v[2].shininess);
	colorC.z = v[2].color_diffuse[2]  * 255.0f * Dot(lightA, normalA) + v[2].color_specular[2]  * 255.0f * pow(Dot(reflectionA, camA), v[2].shininess);
	Multiply(colorC, ratioC);

	color.x = (colorA.x + colorB.x + colorC.x);
	color.y = (colorA.y + colorB.y + colorC.y);
	color.z = (colorA.z + colorB.z + colorC.z);

}