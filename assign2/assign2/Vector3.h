#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3(float vx, float vy, float vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}
};

void crossProduct(Vector3 v1, Vector3 v2, Vector3 &out)
{
	out.x = v1.y * v2.z - v1.z * v2.y;
	out.y = v1.z * v2.x - v1.x *  v2.z;
	out.z = v1.x * v2.y - v1.y * v2.x;
}

void Normalize(Vector3 &v)
{
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x /= length;
	v.y /= length;
	v.z /= length;
}

#endif