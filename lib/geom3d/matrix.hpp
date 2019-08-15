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

#ifndef MATRIX_H
#define MATRIX_H
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_statistics_double.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory>
#include <tuple>
#include "helper/error.hpp"
using namespace std;

namespace Json {
    class Value;
}

namespace Geom3D {
	class Matrix {
		typedef pair<gsl_matrix*, gsl_vector*> matrix_pair;
		matrix_pair __matrix;
	public:
		typedef tuple<double, double, double, double> matrix_tuple;
		Matrix() : __matrix(make_pair(gsl_matrix_alloc(3, 3), gsl_vector_alloc(3))) {}
		Matrix(const double d[9]) : __matrix(make_pair(gsl_matrix_alloc(3, 3), gsl_vector_alloc(3))) { for (int i=0; i<9; i++) { gsl_matrix_set(__matrix.first, i/3, i%3, d[i]); } gsl_vector_set_all(__matrix.second, 0); }
		Matrix(gsl_matrix *U, gsl_vector *t) : __matrix(make_pair(gsl_matrix_alloc(3, 3), gsl_vector_alloc(3))) { gsl_matrix_memcpy(__matrix.first, U); gsl_vector_memcpy(__matrix.second, t);}
		Matrix(const Matrix &other) { __matrix = make_pair(gsl_matrix_alloc(3, 3), gsl_vector_alloc(3)); gsl_matrix_memcpy(__matrix.first, other.rota()); gsl_vector_memcpy(__matrix.second, other.trans()); } // copy constructor
        Matrix& operator=(const Matrix &other) { gsl_matrix_memcpy(__matrix.first, other.rota()); gsl_vector_memcpy(__matrix.second, other.trans()); return *this; } // copy constructor
		Matrix(const Json::Value& rota, const Json::Value& trans);
		~Matrix() { gsl_matrix_free(__matrix.first); gsl_vector_free(__matrix.second); }
		gsl_matrix* rota() const { return __matrix.first; }
		gsl_vector* trans() const { return __matrix.second; }
		void set_row(const int &row, const matrix_tuple &t) {
			//~ cout << "set_row"<<endl;
			//~ exit(1);
		    gsl_matrix_set(__matrix.first, row, 0, std::get<0>(t));
		    gsl_matrix_set(__matrix.first, row, 1, std::get<1>(t));
		    gsl_matrix_set(__matrix.first, row, 2, std::get<2>(t));
			gsl_vector_set(__matrix.second, row,   std::get<3>(t));
		}
		friend ostream& operator<< (ostream& stream, const Matrix& m) {
			gsl_matrix *r = m.rota();
			gsl_vector *v = m.trans();
			stream << "[" << gsl_matrix_get(r, 0, 0) << "," << gsl_matrix_get(r, 0, 1) << "," << gsl_matrix_get(r, 0, 2) << "," << gsl_vector_get(v, 0) << "]" << endl;
			stream << "[" << gsl_matrix_get(r, 1, 0) << "," << gsl_matrix_get(r, 1, 1) << "," << gsl_matrix_get(r, 1, 2) << "," << gsl_vector_get(v, 1) << "]" << endl;
			stream << "[" << gsl_matrix_get(r, 2, 0) << "," << gsl_matrix_get(r, 2, 1) << "," << gsl_matrix_get(r, 2, 2) << "," << gsl_vector_get(v, 2) << "]" << endl;
			return stream;
		}
	};
};
#endif