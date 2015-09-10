#include "partial.hpp"
#include "state.hpp"
#include "score/score.hpp"
#include "pdbreader/molecule.hpp"
#include "pdbreader/bond.hpp"
#include "helper/benchmark.hpp"
#include "helper/help.hpp"
#include "helper/array2d.hpp"
#include "graph/mcqd.hpp"
#include <queue>
#include <algorithm>

using namespace std;

namespace Linker {

	ostream& operator<<(ostream& os, const Partial &le)	{
		os << "start link ++++++++++++++++++++++++++++++" << endl;
		os << "ENERGY = " << le.get_energy() << endl;
		for (auto &pstate : le.get_states())
			//~ os << *pstate << endl;
			os << pstate->pdb() << endl;
		os << "end link --------------------------------" << endl;
		return os;
	}

	ostream& operator<<(ostream& os, const Partial::Vec &vec_le)	{
		for (auto &le : vec_le) {
			os << le << endl;
		}			
		return os;
	}

	//~ double Partial::compute_rmsd_ord(const Partial &other) const {
		//~ Geom3D::Point::Vec crds1, crds2;
		//~ for (auto &pstate1 : this->get_states()) {
			//~ for (auto &pstate2 : other.get_states()) {
				//~ if (pstate1->get_segment().get_id() == pstate2->get_segment().get_id()) {
					//~ for (auto &crd : pstate1->get_crds()) {
						//~ crds1.push_back(crd);
					//~ }
					//~ for (auto &crd : pstate2->get_crds()) {
						//~ crds2.push_back(crd);
					//~ }
				//~ }
			//~ }
		//~ }
		//~ return sqrt(Geom3D::compute_rmsd_sq(crds1, crds2));
	//~ }
//~ 
	double Partial::compute_rmsd_ord(const Partial &other) const {
		double sum_squared = 0;
		int sz = 0;
		for (auto &pstate1 : this->get_states()) {
			for (auto &pstate2 : other.get_states()) {
				if (pstate1->get_segment().get_id() == pstate2->get_segment().get_id()) {
					if (pstate1->get_id() == pstate2->get_id()) {
						sz += pstate1->get_crds().size();
						//~ cout << "cheap" << endl;
					} else  {
						auto &crds1 = pstate1->get_crds();
						auto &crds2 = pstate2->get_crds();
						for (int i = 0; i < crds1.size(); ++i) {
							sum_squared += crds1[i].distance_sq(crds2[i]);
						}
						sz += crds1.size();
					}
				}
			}
		}
		return sqrt(sum_squared / sz);
	}

	Geom3D::Point Partial::compute_geometric_center() const { 
		return Geom3D::compute_geometric_center(this->get_ligand_crds()); 
	}

	void Partial::sort(Partial::Vec &v) {
		::sort(v.begin(), v.end(), Partial::comp());
	}

	Geom3D::Point::Vec Partial::get_ligand_crds() const { 
		Geom3D::Point::Vec crds;
		for (auto &pstate: __states) {
			for (auto &crd : pstate->get_crds()) {
				crds.push_back(crd);
			}
		}
		return crds; 
	}

	void Partial::set_ligand_crds(const Geom3D::Point::Vec &crds) { 
		int i = 0;
		for (auto &pstate: __states) {
			for (auto &crd : pstate->get_crds()) {
				crd = crds[i++];
			}
		}
	}

	Molib::Atom::Vec Partial::get_ligand_atoms() { 
		Molib::Atom::Vec atoms; 
		for (auto &pstate: __states) {
			for (auto &patom : pstate->get_segment().get_atoms()) {
				atoms.push_back(patom);
			}
		}
		return atoms;
	}

}