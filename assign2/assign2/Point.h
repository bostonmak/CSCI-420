#ifndef POINT_H
#define POINT_H

#include "Spline.h"
#include <math.h>

/* represents one control point along the spline */
struct Point {
	double x;
	double y;
	double z;

	float Distance(Point p) {
		float xDist = p.x - x;
		float yDist = p.y - y;
		float zDist = p.z - z;
		return sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
	}

};


#endif POINT_H