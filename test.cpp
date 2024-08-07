
#include "param.h"

int main(int argc, char* argv[]) {
    params::Args args;
    using namespace params;
    args.set_rule("--exist", "", "", "", true);
    std::cout << std::boolalpha << (args.exists("--exist")) << std::endl;
    if (!args.analyze(argc, argv)) {
        return 1;
    }
    bool v;
    std::cout << (args.get("--exist", v), ", v:", v) << std::endl;
    std::cout << (args.get_exe_path()) << std::endl;
    // for (const auto& vv : v) {
    //     DEBUG("vv=", vv);
    // }
    std::cout << "exist --exist?: " << std::boolalpha << args.exists({"--exist"})
              << "\texist 456?: " << args.exists({"456"})
              << "\texist 4564?: " << args.exists({"4564"})
              << "\texist daf?: " << args.exists({"daf"}) << std::endl;
    return 0;
}
