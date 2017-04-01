#include <iostream>
#include "program/cmdlnopts.hpp"
#include "helper/benchmark.hpp"
#include "program/target.hpp"

#include "version.hpp"


using namespace std;

////////////////// GENERATE POTENTIAL FUNCTIONS ///////////////////////////

int main(int argc, char* argv[]) {
        try {
                help::Options::set_options( new Program::CmdLnOpts( 
                    argc, argv, Program::CmdLnOpts::SCORING));

                Benchmark main_timer;
                main_timer.display_time("Starting");

                cout << Version::get_banner()   <<
                        Version::get_version()  <<
                        Version::get_run_info() <<
                        help::Options::get_options()->configuration_file() << endl;

                Program::Target::make_objective();

                main_timer.display_time("Finished");
        } catch (exception& e) {
                cerr << e.what() << endl;
        }
        return 0;
}
