#include <iostream>
#include "program/cmdlnopts.hpp"
#include "version.hpp"
#include "helper/logger.hpp"

////////////////// PRINT OUT CANDOCK CONFIGURATION FILE ///////////////////////////

using namespace std;

int main(int argc, char* argv[]) {
        try {
                if(!drm::check_drm()) {
                    throw logic_error("CANDOCK has expired. Please contact your CANDOCK distributor to get a new version.");
                }
                
                help::Options::set_options(new Program::CmdLnOpts(argc, argv));

                cout << help::Options::get_options()->configuration_file() << endl;
                
        } catch (exception& e) {
                log_error << e.what() << endl;
                return 1;
        }
        return 0;
}
