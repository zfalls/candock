#include "program/target.hpp"

#include "score/score.hpp"
#include "modeler/forcefield.hpp"
#include "modeler/modeler.hpp"

#include "version.hpp"
#include "drm/drm.hpp"

#include "fileout/fileout.hpp"

#include "molecularbondextractor.hpp"
#include "molecularbondbinner.hpp"

using namespace std;
using namespace Program;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
        try {
                if(!drm::check_drm(Version::get_install_path() + "/.candock")) {
                    throw logic_error("CANDOCK has expired. Please contact your CANDOCK distributor to get a new version.");
                }

                Inout::Logger::set_all_stderr(true);

                boost::program_options::variables_map vm;
                po::options_description cmdln_options;

                po::options_description generic;
                generic.add_options()
                ("help,h", "Show this help");

                po::options_description extract_file_options ("Molecular Bond Extraction Files");
                extract_file_options.add_options()
                ("num_mols_to_read", po::value<int>()->default_value(10),
                 "Number of molecules to read from a file at a time.")
                ("stretch_extract_file", po::value<std::string> ()->default_value("stretches.tbl"),
                 "File to save stretches")
                ("angle_extract_file", po::value<std::string> ()->default_value("angles.tbl"),
                 "File to save angles")
                ("dihedral_extract_file", po::value<std::string> ()->default_value("dihedrals.tbl"),
                 "File to save dihedrals")
                ("improper_extract_file", po::value<std::string> ()->default_value("impropers.tbl"),
                 "File to save impropers")
                ;

                po::options_description binning_options ("Bond Bin Files");
                binning_options.add_options()
                ("stretch_bin_size,s", po::value<double> ()->default_value(0.01),
                 "Stretch bin size")
                ("angle_bin_size,a", po::value<double> ()->default_value(0.01),
                 "Angle bin size")
                ("dihedral_bin_size,d", po::value<double> ()->default_value(0.01),
                 "Dihedral bin size")
                ("improper_bin_size,i", po::value<double> ()->default_value(0.01),
                 "Improper bin size")
                ;

                po::options_description bin_file_options ("Bond Bin Sizes");
                bin_file_options.add_options()
                ("stretch_bin_out", po::value<std::string> ()->default_value("stretch_bins.tbl"),
                 "File to save stretches")
                ("angle_bin_out", po::value<std::string> ()->default_value("angle_bins.tbl"),
                 "File to save angles")
                ("dihedral_bin_out", po::value<std::string> ()->default_value("dihedral_bins.tbl"),
                 "File to save dihedrals")
                ("improper_bin_out", po::value<std::string> ()->default_value("improper_bins.tbl"),
                 "File to save impropers")
                ;

                cmdln_options.add(generic);
                cmdln_options.add(extract_file_options);
                cmdln_options.add(binning_options);
                cmdln_options.add(bin_file_options);

                po::store (po::parse_command_line (argc, argv, cmdln_options), vm);
                po::notify (vm);

                if (vm.count ("help")) {
                        std::cout << cmdln_options << std::endl;
                        return 0;
                }

                std::ofstream stretch_file  (vm["stretch_extract_file"].as<std::string>(),  std::ios::app);
                std::ofstream angle_file    (vm["angle_extract_file"].as<std::string>(),    std::ios::app);
                std::ofstream dihedral_file (vm["dihedral_extract_file"].as<std::string>(), std::ios::app);
                std::ofstream improper_file (vm["improper_extract_file"].as<std::string>(), std::ios::app);

                int num_mols_to_read = vm["num_mols_to_read"].as<int>();

                double stretch_bin = vm["stretch_bin_size"].as<double>();
                double angle_bin = vm["angle_bin_size"].as<double>();
                double dihedral_bin = vm["dihedral_bin_size"].as<double>();
                double improper_bin = vm["improper_bin_size"].as<double>();

                AtomInfo::MolecularBondBinner MBB(stretch_bin,
                                                  angle_bin,
                                                  dihedral_bin,
                                                  improper_bin);

                for (int i = 1; i < argc; ++i) {
                        Parser::FileParser input(argv[i],
                                                 Parser::pdb_read_options::all_models,
                                                 num_mols_to_read
                                                );
                        Molib::Molecules mols;

                        AtomInfo::MolecularBondExtractor MBE;

                        while (input.parse_molecule(mols)) {

                            mols.compute_idatm_type();
                            mols.erase_hydrogen();

                            for ( const auto& mol : mols ) {
                                if (!MBE.addMolecule(mol))
                                    continue;

                                MBE.printStretches(stretch_file);
                                MBE.printAngles(angle_file);
                                MBE.printDihedrals(dihedral_file);
                                MBE.printImpropers(improper_file);
                            }

                            MBB.addExtracts(MBE);
                        }
                }

                stretch_file.close();
                angle_file.close();
                dihedral_file.close();
                improper_file.close();

                std::ofstream stretch_bin_file  (vm["stretch_bin_out"].as<std::string>());
                std::ofstream angle_bin_file    (vm["angle_bin_out"].as<std::string>());
                std::ofstream dihedral_bin_file (vm["dihedral_bin_out"].as<std::string>());
                std::ofstream improper_bin_file (vm["improper_bin_out"].as<std::string>());

                MBB.printStretchBins(stretch_bin_file);
                MBB.printAngleBins(angle_bin_file);
                MBB.printDihedralBins(dihedral_bin_file);
                MBB.printImproperBins(improper_bin_file);

        } catch (po::error& e) {
                std::cerr << "error: " << e.what() << std::endl;
                std::cerr << "Please see the help (-h) for more information" << std::endl;
        } catch (exception& e) {
                cerr << e.what() << endl;
        }
        return 0;
}