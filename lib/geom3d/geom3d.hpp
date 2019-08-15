/* Copyright (c) 2016-2019 Chopra Lab at Purdue University, 2013-2016 Janez Konc at National Institute of Chemistry and Samudrala Group at University of Washington
 *
 * This program is free for educational and academic use
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

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
        double compute_rmsd(const Point::Vec &crds1, const Point::Vec &crds2);

        Point compute_geometric_center(const Geom3D::Point::Vec &crds);
        Point::Vec uniform_sphere(const int n);

        ostream& operator<<(ostream& os, const Geom3D::Point::Vec &points);
        ostream& operator<<(ostream& os, const Geom3D::Point::ConstSet &points);
}
#endif