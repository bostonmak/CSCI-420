#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

struct Vector3 
{
	double x;
	double y;
	double z;

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3(double vx, double vy, double vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}

	friend Vector3 operator+(Vector3 lhs, Vector3 rhs)
	{
		return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	friend Vector3 operator-(Vector3 lhs, Vector3 rhs)
	{
		return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	float Magnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	static Vector3 Zero();
};

void CrossProduct(Vector3 v1, Vector3 v2, Vector3 &out)
{
	out.x = v1.y * v2.z - v1.z * v2.y;
	out.y = v1.z * v2.x - v1.x *  v2.z;
	out.z = v1.x * v2.y - v1.y * v2.x;
}

double Dot(Vector3 v1, Vector3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void Normalize(Vector3 &v)
{
	double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x /= length;
	v.y /= length;
	v.z /= length;
}

Vector3 Add(Vector3 v1, Vector3 v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

void Multiply(Vector3 &v, double f)
{
	v.x *= f;
	v.y *= f;
	v.z *= f;
}

Vector3 Vector3::Zero()
{
	return Vector3(0, 0, 0);
}

#endif