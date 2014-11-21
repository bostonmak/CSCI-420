
#pragma once

struct Ray
{
	Vector3 origin;
	Vector3 direction;
	double range;

	Ray()
	{
		origin = Vector3::Zero();
		direction = Vector3::Zero();
		range = 0;
	}
	Ray(Vector3 orig, Vector3 dir, double dist)
	{
		origin = orig;
		direction = dir;
		range = dist;
	}

	bool IntersectsTriangle(Triangle t, Vector3& out, Vector3& normal);
	bool IntersectsSphere(Sphere s, Vector3& out, Vector3& normal);
};

bool Ray::IntersectsTriangle(Triangle t, Vector3& out, Vector3& normal)
{
	Vector3 vertexA = Vector3(t.v[0].position[0], t.v[0].position[1], t.v[0].position[2]);
	Vector3 vertexB = Vector3(t.v[1].position[0], t.v[1].position[1], t.v[1].position[2]);
	Vector3 vertexC = Vector3(t.v[2].position[0], t.v[2].position[1], t.v[2].position[2]);
	CrossProduct(vertexB - vertexA, vertexC - vertexA, normal);
	Normalize(normal);
	// n dot (p' - p)
	double numerator = Dot(normal, vertexA - origin);
	// n dot r
	double denominator = Dot(normal, direction);
	if (std::abs(denominator) < DBL_EPSILON)
	{
		return false;
	}
	double tVal = numerator / denominator;
	if (tVal <= 0 || tVal > range)
	{
		return false;
	}
	Vector3 tProgression = direction;
	Multiply(tProgression, tVal);
	out = origin + tProgression;

	Vector3 windingA, windingB, windingC;
	CrossProduct(vertexB - vertexA, out - vertexA, windingA);
	CrossProduct(vertexC - vertexB, out - vertexB, windingB);
	CrossProduct(vertexA - vertexC, out - vertexC, windingC);
	double dotA = Dot(normal, windingA);
	double dotB = Dot(normal, windingB);
	double dotC = Dot(normal, windingC);
	if (dotA > 0 && dotB > 0 && dotC > 0)
	{
		return true;
	}

	return false;
}

bool Ray::IntersectsSphere(Sphere s, Vector3& out, Vector3& normal)
{
	Vector3 center = Vector3(s.position[0], s.position[1], s.position[2]);
	double b = 2 * (direction.x * (origin.x - center.x) + direction.y * (origin.y - center.y) + direction.z * (origin.z - center.z));
	double c = (origin.x - center.x) * (origin.x - center.x) + (origin.y - center.y) * (origin.y - center.y) + (origin.z - center.z) * (origin.z - center.z) - s.radius * s.radius;
	double t0 = (-b + sqrtf(b * b - 4 * c)) / 2;
	double t1 = (-b - sqrtf(b * b - 4 * c)) / 2;

	if ((b * b - 4 * c) < 0)
		return false;

	if (t0 >= 0 || t1 >= 0)
	{
		if (t0 >= 0 && t1 >= 0)
		{
			double t = t0 < t1 ? t0 : t1;
			Vector3 tProgression = direction;
			Multiply(tProgression, t);
			out = origin + tProgression;
			normal = out - center;
			return true;
		}
		else if (t0 >= 0)
		{
			Vector3 tProgression = direction;
			Multiply(tProgression, t0);
			out = origin + tProgression;
			normal = out - center;
			return true;
		}
		else
		{
			Vector3 tProgression = direction;
			Multiply(tProgression, t1);
			out = origin + tProgression;
			normal = out - center;
			return true;
		}
	}
	return false;
}