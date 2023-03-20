#include "test_env.hpp"
#include <sstream>

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> SOURCES\n"
        << "Options:\n"
        << "\t-h,--help\t\tShow this help message\n"
        << "\t-d,--destination DESTINATION\tSpecify the destination path"
        << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    std::vector <std::string> sources;
    std::string destination;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
        else if ((arg == "-r") || (arg == "--run")) {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                destination = argv[i++]; // Increment 'i' so we don't get the argument as the next argv[i].
            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "--run option requires one argument." << std::endl;
                return 1;
            }
        }
        else if ((arg == "-t") || (arg == "--times")) {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                destination = argv[i++]; // Increment 'i' so we don't get the argument as the next argv[i].
            }
            else { // Uh-oh, there was no argument to the destination option.
                std::cerr << "--time option requires one argument." << std::endl;
                return 1;
            }
        }
        else {
            sources.push_back(argv[i]);
        }
    }

    for (auto i : sources) {
        if (i.compare(0, 6, "--run=") == 0) {
            std::string test = i.substr(6);
            std::stringstream ss_test(test);
            std::string segment;
            std::vector<std::string> seglist;
            while (std::getline(ss_test, segment, ',')){
                seglist.push_back(segment);
            }
            for (const auto k : seglist) {
                std::string::size_type j = k.find("/");
                if (j != std::string::npos) {
                    std::string group = k.substr(0, j);
                    std::string function = k.substr(j + 1);
                    tester::instance().run(group.c_str(), function.c_str());
                }
                else {
                    tester::instance().run(k.c_str());
                }
            }
        }
    }

    return 0;
}
