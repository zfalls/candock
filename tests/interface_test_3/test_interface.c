#include <candock/interface/interface.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main( int argc, char **argv) {

        if (argc < 3) {
               puts("Error, please give at least 3 arguments\n");
               return 1;
        }

        clock_t start = clock();

		printf("Starting up \n");

        if (! initialize_complex(argv[1])) {
                printf("%s\n", cd_get_error());
                return 1;
        }

        if (! initialize_scoring(argv[2])) {
                printf("%s\n", cd_get_error());
                return 1;
        }

		printf("Number of receptor atoms: %d\n", receptor_atom_count());
		printf("Number of ligand atoms: %d\n", ligand_atom_count());
        printf("Time to initialize: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        start = clock();
        printf("Score before setting: %f\n", calculate_score());
        printf("Time to do one scoring: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        size_t  lig_atom_count = ligand_atom_count();
        size_t* idx = (size_t*)malloc( lig_atom_count * sizeof(size_t) );
        float * positions = (float*)malloc( lig_atom_count * sizeof(float) * 3 );
        ligand_atoms(idx, positions);

        for (int i = 0; i < lig_atom_count; ++i) {
                positions[ i * 3 + 0 ] = positions[ i * 3 + 0 ] + 0.5f;
                positions[ i * 3 + 1 ] = positions[ i * 3 + 1 ] + 0.5f;
                positions[ i * 3 + 2 ] = positions[ i * 3 + 2 ] + 0.5f;
        }

        start = clock();

        set_positions_ligand( idx, positions, lig_atom_count );
        
        printf("Time to set ligand: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        start = clock();
        printf("Score after setting ligand: %f\n", calculate_score());
        printf("Time to score again: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        size_t rec_atom_count = receptor_atom_count();
        size_t* idx_rec = (size_t*)malloc(rec_atom_count * sizeof(size_t));
        float * positions_rec = (float*)malloc(rec_atom_count * sizeof(float) * 3);
        receptor_atoms(idx_rec, positions_rec);

        for (int j = 0; j < rec_atom_count; ++j) {
                positions_rec[ j * 3 + 0 ] = positions_rec[ j * 3 + 0 ] + 0.5f;
                positions_rec[ j * 3 + 1 ] = positions_rec[ j * 3 + 1 ] + 0.5f;
                positions_rec[ j * 3 + 2 ] = positions_rec[ j * 3 + 2 ] + 0.5f;
        }

        start = clock();
        set_positions_receptor(idx_rec, positions_rec, rec_atom_count);
        printf("Time to set receptor: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        start = clock();
        printf("Score after setting receptor: %f\n", calculate_score());
        printf("Time to score again: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        start = clock();
        size_t lig_bond_count = ligand_bond_count();
        size_t* lig_bonds = (size_t*)malloc(lig_bond_count * sizeof(size_t) * 3);
        ligand_bonds(lig_bonds);
        printf("Time to get ligand bonds: %f\n", (double) ( clock() - start) / CLOCKS_PER_SEC );

        for ( size_t k = 0; k < lig_bond_count; ++k ) {
                printf ("BOND: %zu %zu %zX\n", lig_bonds[ k * 3 + 0 ],
                                               lig_bonds[ k * 3 + 1 ],
                                               lig_bonds[ k * 3 + 2 ] );
        }

        return 0;
}
