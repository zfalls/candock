
#include <iostream>
#include "program/cmdlnopts.hpp"
#include "program/target.hpp"
#include "version.hpp"
#include "drm/drm.hpp"

////////////////// BINDING SITE DETECTION USING PROBIS ///////////////////////////

using namespace std;

int main(int argc, char* argv[]) {
        try {
                if(!drm::check_drm(Version::get_install_path() + "/.candock")) {
                    throw logic_error("CANDOCK has expired. Please contact your CANDOCK distributor to get a new version.");
                }

                help::Options::set_options( new Program::CmdLnOpts(
                    argc, argv, Program::CmdLnOpts::STARTING |
                                Program::CmdLnOpts::PROBIS));

                Benchmark main_timer;
                main_timer.display_time("Starting");

                cout << Version::get_banner()   <<
                        Version::get_version()  <<
                        Version::get_run_info();
                cout << help::Options::get_options()->configuration_file() << endl;


                Program::Target targets (cmdl.get_string_option("receptor"));
                targets.find_centroids();

                main_timer.display_time("Finished");


        } catch (exception& e) {
                cerr << e.what() << endl;
                return 1;
        }
        return 0;
}
