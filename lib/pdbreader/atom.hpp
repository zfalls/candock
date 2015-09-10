#ifndef ATOM_H
#define ATOM_H
#include <memory>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/ip/host_name.hpp>
#include "geom3d/coordinate.hpp"
#include "geom3d/matrix.hpp"
#include "it.hpp"
#include "element.hpp"
#include "helper/help.hpp"
#include "grid.hpp"
#include "bond.hpp"
#include "graph/graph.hpp"

using namespace std;

namespace Molib {
	class Residue;
	
	class Atom : public template_vector_container<Atom*, Atom> {
	public:
		typedef tuple<int, string, unique_ptr<Geom3D::Coordinate>, double> atom_tuple;
		typedef pair<Atom*, Atom*> Pair;
		typedef vector<Atom*> Vec;
		typedef set<Atom*> Set;
		typedef ::Grid<Atom> Grid;
		typedef Glib::Graph<Atom> Graph;

	private:
		int __atom_number;
		string __atom_name;
		Geom3D::Coordinate __crd;
		int __idatm_type;
		string __gaff_type;
		Element __element;
		string __smiles_label;
		map<string, int> __smiles_prop;
		map<int, int> __aps;
		void *__br; // back reference
		map<Atom*, shared_ptr<Bond>> __bonds;
		
	public:
		Atom(const Atom &rhs) : __atom_number(rhs.__atom_number), 
			__atom_name(rhs.__atom_name), __crd(rhs.__crd), 
			__idatm_type(rhs.__idatm_type), __gaff_type(rhs.__gaff_type),
			__element(rhs.__element), __smiles_label(rhs.__smiles_label),
			__smiles_prop(rhs.__smiles_prop), __aps(rhs.__aps), __br(nullptr) {
			dbgmsg("Copy constructor : atom");
		}

		Atom(const int atom_number, const string &smiles_label, 
			const map<string, int> smiles_prop) : __atom_number(atom_number), 
			__smiles_label(smiles_label), __smiles_prop(smiles_prop), __element(""),
			__br(nullptr) {}
		Atom(const Geom3D::Coordinate &crd) : __crd(crd), __element("") {}
		Atom(const Geom3D::Coordinate &crd, const int &idatm_type) : __crd(crd), 
			__idatm_type(idatm_type), __element("") {}
		// if element is missing, try to guess it from atom name
		Atom(int atom_number, const string &atom_name, const Geom3D::Coordinate &crd, 
			const int idatm_type, const string &element="") : __atom_number(atom_number), 
			__atom_name(atom_name), __crd(crd), __idatm_type(idatm_type), 
			__gaff_type("???"), __element(element == "" ? atom_name : element) {}			
		Bond& connect(Atom &a2);
		void clear_bonds() { __bonds.clear(); }
		BondVec get_bonds() const { BondVec bonds; for (auto &kv : __bonds) 
			bonds.push_back(&*kv.second); return bonds; }
		const shared_ptr<Bond>& get_shared_ptr_bond(Atom &other) const { return __bonds.at(&other); }
		Bond& get_bond(Atom &other) const { return *get_shared_ptr_bond(other); }
		shared_ptr<Bond>& insert_bond(Atom &other, const shared_ptr<Bond> &bond) { return __bonds.insert({&other, bond}).first->second; }
		shared_ptr<Bond>& insert_bond(Atom &other, Bond *bond) { return __bonds.insert({&other, shared_ptr<Bond>(bond)}).first->second; }
		void erase_bond(Atom &other) { __bonds.erase(&other); }
		bool is_adjacent(Atom &other) const { return __bonds.count(&other); }
		bool is_adjacent(const string &atom_name) const { for (auto &other : *this) if (other.atom_name() == atom_name) return true; return false; }
		int get_num_hydrogens() const;
		int atom_number() const { return __atom_number; }
		void set_atom_name(const string &atom_name) { __atom_name = atom_name; }
		void set_atom_number(int atom_number) { __atom_number = atom_number; }
		void set_idatm_type(const string &idatm_type) { __idatm_type = help::idatm_mask.at(idatm_type); }
		void set_gaff_type(const string &gaff_type) { __gaff_type = gaff_type; }
		Atom& insert_property(const string &prop, const int count) { __smiles_prop.insert({prop, count}); return *this; }
		Atom& add_property(const string &prop) { __smiles_prop[prop]++; return *this; }
		Atom& erase_property(const string &prop) { __smiles_prop.erase(prop); return *this; }
		Atom& erase_properties() { __smiles_prop.clear(); return *this; }
		bool has_property(const string &prop) const { return __smiles_prop.count(prop); }
		int get_num_property(const string &prop) const { 
			dbgmsg("__smiles prop(" << prop << ") count = " << __smiles_prop.count(prop)); 
			if (__smiles_prop.count(prop) == 0) return 0;
			return __smiles_prop.at(prop); }
		int get_num_bond_with_bond_gaff_type(const string &prop) const;
		int compute_num_property(const string &prop) const;
		void set_crd(const Geom3D::Coordinate &crd) { __crd = crd; }
		string idatm_type_unmask() const { return help::idatm_unmask[__idatm_type]; }
		int idatm_type() const { return __idatm_type; }
		double radius() const { return help::vdw_radius[__idatm_type]; }
		const string& gaff_type() const { return __gaff_type; }
		const string& atom_name() const { return __atom_name; }
		Element element() const { return __element; }
		Geom3D::Coordinate& crd() { return __crd; }
		const Geom3D::Coordinate& crd() const { return __crd; }
		friend ostream& operator<< (ostream& stream, const Atom& a);
		double distance() const { return 0.0; } // just dummy : needed by grid
		void distance(double d) const {} // just dummy : needed by grid
		const map<int, int>& get_aps() const { return __aps; }
		void set_members(const string &str);
		const Residue& br() const { return *static_cast<const Residue*>(__br); }
		void set_br(void *br) { __br = br; }
		// the following are required for Atom::Graph :-)
		bool compatible(const Atom &atom) const;
		string get_label() const { return (__smiles_label.empty() 
			? help::idatm_unmask[__idatm_type] : __smiles_label); }
		const int weight() const { return 0; } // dummy for graph ostream operator

		static Graph create_graph(const Vec &atoms);
		static Graph create_graph(const Set &atoms);

	};

	BondSet get_bonds_in(const Atom::Set &atoms, bool in=true);
	BondSet get_bonds_in(const Atom::Vec &atoms, bool in=true);

	ostream& operator<< (ostream& stream, const Atom::Set &atoms);
	ostream& operator<< (ostream& stream, const Atom::Vec &atoms);
};
#endif