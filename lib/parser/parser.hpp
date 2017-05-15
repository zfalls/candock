#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "molib/molecules.hpp"

namespace Parser {
        enum pdb_read_options {
                first_model=1,
                all_models=2,
                hydrogens=4,
                skip_hetatm=8,
                skip_atom=16,
                sparse_macromol=32,
                docked_poses_only=64,
        };

        class Parser {
        protected:
                const std::string __molecule_file;
                unsigned int __hm;
                const int __num_occur;
                std::streampos __pos;
                bool __giant_molecule;
                void __generate_molecule(Molib::Molecules&, bool&, const std::string&);
                void __generate_assembly(Molib::Molecules&, bool&, int, const std::string&);
                void __generate_model(Molib::Molecules&, bool&, int);
        public:
                Parser(const string &molecule_file, unsigned int hm= all_models, 
                       const int num_occur=-1)
                       : __molecule_file(molecule_file), __hm(hm), 
                         __num_occur(num_occur), __pos(0), __giant_molecule(false) {}
                virtual ~Parser() {}
                virtual void parse_molecule(Molib::Molecules&) = 0;
                virtual void set_pos(std::streampos pos);
                virtual void set_hm(unsigned int hm);
        };
}

#endif // PARSER_H
