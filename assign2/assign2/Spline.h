#ifndef SPLINE_H
#define SPLINE_H

#include "Point.h"

struct Spline {
	int numControlPoints;
	struct Point *points;
};

void createControlMatrix(int index, Spline spline, float *matrixOut) {
	if (index > 0 && index < spline.numControlPoints - 1) {
		Point p;
		for (int i = 0; i < 4; i++) {
			p = spline.points[index + i - 1];
			matrixOut[3 * i] = p.x;
			matrixOut[(3 * i) + 1] = p.y;
			matrixOut[(3 * i) + 2] = p.z;
		}
	}
}

void interpolatePoints(Point p1, Point p2, std::vector<Point> &pOut, 
	float *basisMatrix, float *controlMatrix, float maxDistance, int &index, float &u) {

	if (p1.Distance(p2) >= maxDistance) {
		float uVector[4] = { u * u * u, u * u, u, 1 };
		float uBasis[4] = { 0, 0, 0, 0 };
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				uBasis[i] = uBasis[i] + uVector[j] * basisMatrix[(j * 4) + i];
			}
		}
		float mid[3] = { 0, 0, 0 };
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				mid[i] = mid[i] + uBasis[j] * controlMatrix[(j * 3) + i];
			}
		}
		Point midPoint;
		midPoint.x = mid[0];
		midPoint.y = mid[1];
		midPoint.z = mid[2];
		u /= 2;
		interpolatePoints(p1, midPoint, pOut, basisMatrix, controlMatrix, maxDistance, index, u);
		pOut.push_back(midPoint);
		index++;
		interpolatePoints(midPoint, p2, pOut, basisMatrix, controlMatrix, maxDistance, index, u);

	}

}

#endif