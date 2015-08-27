#include "segment.hpp"
#include "pdbreader/molecule.hpp"
#include "helper/benchmark.hpp"
#include "helper/help.hpp"

namespace Linker {
	ostream& operator<< (ostream& stream, const Segment& s) {
		stream << "Segment(" << s.__seed_id << ") : atom numbers = ";
		for (auto &pa : s.__atoms) stream << pa->atom_number() << " ";
		return stream;
	}

	Segment::Segment(const Molib::Atom::Vec atoms, const int &seed_id, const Segment::Id idx) 
		: __atoms(atoms), __seed_id(seed_id), __id(idx), __join_atom(atoms.size(), false) { 

		for (int i = 0; i < atoms.size(); ++i) 
			__amap[atoms[i]] = i; 
	}

	//~ const Molib::Atom& Segment::adjacent_in_segment(const Molib::Atom &atom, 
		//~ const Molib::Atom &forbidden) const { 
		//~ for (auto &adj : atom) {
			//~ if (&adj != &forbidden && has_atom(adj)) 
				//~ return adj; 
		//~ }
		//~ throw Error("die : couldn't find adjacent in segment");
	//~ }
//~ 
	const int Segment::adjacent_in_segment(const Molib::Atom &atom, 
		const Molib::Atom &forbidden) const { 
		for (auto &adj : atom) {
			if (&adj != &forbidden && has_atom(adj)) 
				return get_idx(adj); 
		}
		throw Error("die : couldn't find adjacent in segment");
	}

	void Segment::set_bond(const Segment &other, Molib::Atom &a1, Molib::Atom &a2) { 
		__bond.insert({&other, Molib::Bond(&a1, &a2, get_idx(a1), get_idx(a2))}); 
	}
	
	Segment::Graph Segment::create_graph(const Molib::Molecule &molecule) {
		dbgmsg("Create segment graph ...");
		const Molib::Model &model = molecule.first().first();
		vector<unique_ptr<Segment>> vertices;
		dbgmsg(model.get_rigid());
		int idx = 0;
		for (auto &fragment : model.get_rigid()) { // make vertices (segments) of a graph
			dbgmsg(fragment.get_all());
			auto all = fragment.get_all();
			Molib::Atom::Vec fragatoms(all.begin(), all.end());
			//~ vertices.push_back(unique_ptr<Segment>(new Segment(fragatoms, fragment.get_seed_id())));
			vertices.push_back(unique_ptr<Segment>(new Segment(fragatoms, fragment.get_seed_id(), idx++)));
		}
		// connect segments
		for (int i = 0; i < vertices.size(); ++i) {
			Segment &s1 = *vertices[i];
			for (int j = i + 1; j < vertices.size(); ++j) {
				Segment &s2 = *vertices[j];
				auto inter = Glib::intersection(s1.get_atoms(), s2.get_atoms());
				dbgmsg(s1.get_atoms().size() << " " << s2.get_atoms().size() << " " << inter.size());
				if (inter.size() == 2) {
					s1.add(&s2);
					s2.add(&s1);
					auto &atom1 = **inter.begin();
					auto &atom2 = **inter.rbegin();
					int num_bonds = 0; 
					for (auto &adj : atom1) {
						if (s1.has_atom(adj))
							num_bonds++;
					}
					if (num_bonds == 1) {
						dbgmsg("atom " << atom1 << " belongs to segment " << s2);
						dbgmsg("atom " << atom2 << " belongs to segment " << s1);
						s1.set_bond(s2, atom2, atom1);
						s2.set_bond(s1, atom1, atom2);
						
						s1.set_join_atom(atom2);
						s2.set_join_atom(atom1);
					} else {
						dbgmsg("atom " << atom1 << " belongs to segment " << s1);
						dbgmsg("atom " << atom2 << " belongs to segment " << s2);
						s1.set_bond(s2, atom1, atom2);
						s2.set_bond(s1, atom2, atom1);

						s1.set_join_atom(atom1);
						s2.set_join_atom(atom2);

					}
				}
			}
		}

		const Segment::Paths paths = __find_paths(vertices);
		__init_max_linker_length(paths);
		__set_branching_rules(paths);

		return Segment::Graph(std::move(vertices), true, false);
	}

	Segment::Paths Segment::__find_paths(const vector<unique_ptr<Segment>> &segments) {
		/*
		 * Find ALL paths between ALL seed segments (even non-adjacent) 
		 * and seeds and leafs
		 */
		Segment::Paths paths;
		dbgmsg("find all paths in a graph");
		Segment::Set seeds, seeds_and_leafs;
		for (auto &pseg : segments) {
			auto &seg = *pseg;
			if (seg.is_seed())
				seeds.insert(&seg);
			if (seg.is_seed() || seg.is_leaf())
				seeds_and_leafs.insert(&seg);
		}
		set<Segment::ConstPair> visited;
		for (auto &pseg1 : seeds) {
			for (auto &pseg2 : seeds_and_leafs) {
				if (pseg1 != pseg2 && !visited.count({pseg1, pseg2})) {
					dbgmsg("finding path between " << *pseg1 << " and "
						<< *pseg2);
					visited.insert({pseg1, pseg2});
					visited.insert({pseg2, pseg1});
					Segment::Graph::Path path = Glib::find_path(*pseg1, *pseg2);
					paths.insert({{pseg1, pseg2}, path });
					dbgmsg("path between first segment " << *pseg1
						<< " and second segment " << *pseg2);
				}
			}
		}
		return paths;
	}

	bool Segment::__link_adjacent(const Segment::Graph::Path &path) {
		/* Returns true if path has no seed segments along the way
		 * 
		 */
#ifndef NDEBUG
		for (auto it = path.begin(); it != path.end(); ++it) {
			dbgmsg(**it << " is seed = " << boolalpha << (*it)->is_seed());
		}
#endif
		for (auto it = path.begin() + 1; it != path.end() - 1; ++it) {
			dbgmsg(**it);
			if ((*it)->is_seed())
				return false;
		}
		return true;
	}

	void Segment::__init_max_linker_length(const Segment::Paths &paths) {
		for (auto &kv : paths) {
			auto &seg_pair = kv.first;
			Segment::Graph::Path path(kv.second.begin(), kv.second.end());
			__compute_max_linker_length(path);
		}
	}

	void Segment::__compute_max_linker_length(Segment::Graph::Path &path) {

		for (int i = 0; i < path.size() - 1; ++i) {

			double d = 0.0;
			
			const Molib::Bond &b1 = path[i]->get_bond(*path[i + 1]);
			Molib::Atom *front_atom = (path[i]->has_atom(b1.atom1()) ? &b1.atom1() : &b1.atom2());

			path[i]->set_max_linker_length(*path[i + 1], b1.length());
			path[i + 1]->set_max_linker_length(*path[i], b1.length());

			dbgmsg("max_linker_length between " << *path[i] << " and " 
				<< *path[i + 1] << " = " << path[i]->get_max_linker_length(*path[i + 1]));
			dbgmsg("MAX_linker_length between " << *path[i] << " and " 
				<< *path[i + 1] << " = " << b1.length());

			for (int j = i + 1; j < path.size() - 1; ++j) {

				const Molib::Bond &b2 = path[j]->get_bond(*path[j + 1]);

				Molib::Atom &b2_atom1 = (path[j]->has_atom(b2.atom1()) ? b2.atom1() : b2.atom2());
				Molib::Atom &b2_atom2 = b2.second_atom(b2_atom1);
		
				double rigid_width = (*front_atom).crd().distance(b2_atom2.crd());
				front_atom = &b2_atom2;

				d += rigid_width;

				path[i]->set_max_linker_length(*path[j + 1], d);
				path[j + 1]->set_max_linker_length(*path[i], d);
				
				dbgmsg("max_linker_length between " << *path[i] << " and " 
					<< *path[j + 1] << " = " << path[i]->get_max_linker_length(*path[j + 1]));
				dbgmsg("MAX_linker_length between " << *path[i] << " and " 
					<< *path[j + 1] << " = " << d);
								
			}
		}
	}
	
	void Segment::__set_branching_rules(const Segment::Paths &paths) {
		for (auto &kv : paths) {
			const Segment::Graph::Path &path = kv.second;
#ifndef NDEBUG
			dbgmsg("valid_path = ");
			for (auto &seg : path) dbgmsg(*seg);
#endif
			Segment &start = *path.back();
			Segment &goal = *path.front();
			Segment &start_next = **(path.end() - 2);
			Segment &goal_next = **(path.begin() + 1);
			const bool is_link_adjacent = __link_adjacent(path);
			for (auto it = path.begin(); it != path.end(); ++it) {
				Segment &current = **it;
				if (&current != &goal && goal.is_seed()) {
					Segment &next = **(it - 1);
					current.set_next(goal, next);
					goal.set_next(current, goal_next);
					if (is_link_adjacent)
						current.set_adjacent_seed_segments(goal);
					dbgmsg("current = " << current << " goal.is_seed() = " 
						<< boolalpha << goal.is_seed() << " next = " << next);
				}
				if (&current != &start && start.is_seed()) {
					Segment &prev = **(it + 1);
					current.set_next(start, prev);
					start.set_next(current, start_next);
					if (is_link_adjacent)
						current.set_adjacent_seed_segments(start);
					dbgmsg("current = " << current << " start.is_seed() = " 
						<< boolalpha << start.is_seed() << " prev = " << prev);
				}
			}
		}
	}
	

};
