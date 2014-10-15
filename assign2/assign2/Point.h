#ifndef POINT_H
#define POINT_H

#include "Spline.h"
#include <math.h>

/* represents one control point along the spline */
struct Point {
	double x;
	double y;
	double z;

	Point()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	float Distance(Point p) {
		return sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y) + (p.z - z) * (p.z - z));
	}

	bool Equals(Point p) {
		if (x == p.x && y == p.y && z == p.z)
		{
			return true;
		}
		return false;
	}
};


#endif POINT_H