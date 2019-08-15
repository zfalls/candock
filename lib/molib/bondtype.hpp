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

#ifndef BONDTYPE_H
#define BONDTYPE_H
#include <memory>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdlib>
#include "it.hpp"

namespace Molib {
	class Atom;
	struct AtomParams {
		int val;
		int aps;
		int con;
	};
	typedef map<Atom*, AtomParams> ValenceState;
	typedef vector<ValenceState> ValenceStateVec;
	typedef map<Bond*, int> BondToOrder;
	
	class BondOrder {
		class BondOrderError : public Error {
		public: 
			BondOrderError(const std::string &msg) : Error(msg) {}
		};
		static ValenceStateVec __create_valence_states(const Atom::Vec &atoms, const int max_valence_states);
		static void __dfs(const int level, const int sum, const int tps, const vector<vector<AtomParams>> &V,
			vector<AtomParams> &Q, vector<vector<AtomParams>> &valence_states, const int max_valence_states);
		static bool __discrepancy(const ValenceState &valence_state);
        // Windows defines the macro __success !
		static bool __my_success(const ValenceState &valence_state);
		static bool __basic_rules(ValenceState &valence_state, BondToOrder &bond_orders);
		static void __trial_error(ValenceState &valence_state, BondToOrder &bond_orders);
		static Bond& __get_first_unassigned_bond(const ValenceState &valence_state, BondToOrder &bond_orders);
	public:
		static void compute_rotatable_bonds(const Atom::Vec &atoms);
		static void compute_bond_order(const Atom::Vec &atoms);
		static void compute_bond_gaff_type(const Atom::Vec &atoms);
	};
	ostream& operator<< (ostream& stream, const ValenceState& valence_state);
	ostream& operator<< (ostream& stream, const ValenceStateVec& valence_states);
	ostream& operator<< (ostream& os, const BondToOrder& bond_orders);
};
#endif