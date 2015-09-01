#ifndef GEOM3D_H
#define GEOM3D_H
#include "coordinate.hpp"
#include <vector>
#include <map>
namespace Geom3D {
	typedef Coordinate Point;
	typedef Coordinate Vector3;
	typedef map<int, Geom3D::Point::Vec> GridPoints;
	double degrees(double);
	double radians(double);
	double angle(const Vector3 &, const Vector3 &);
	double angle(const Point &, const Point &, const Point &);
	double dihedral(const Point &, const Point &, const Point &, const Point &);
	Point line_evaluate(const Point &, const Vector3 &, const double);
	double compute_rmsd_sq(const Point::Vec &crds1, const Point::Vec &crds2);
	Point compute_geometric_center(const Geom3D::Point::Vec &crds);
}
ostream& operator<<(ostream& os, const Geom3D::Point::Vec &points);
#endif
