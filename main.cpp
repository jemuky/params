#include "param.hpp"

int main(int argc, char* argv[]) {
    params::Args args;
    using namespace params;
    params::set_log_lev(LogLevel::Debug);
    args.set_rule("--exist", "", "true", "", true);
    DEBUG(args.exists("--exist"));
    if (!args.analyze(argc, argv)) {
        return 1;
    }
    bool v;
    DEBUG(args.get("--exist", v), ", v:", v);
    DEBUG(args.get_exe_path());
    // for (const auto& vv : v) {
    //     DEBUG("vv=", vv);
    // }
    // std::cout << args.exists({"123"}) << "\t" << args.exists({"456"}) << "\t"
    //           << args.exists({"4564"}) << "\t" << args.exists({"daf"}) << std::endl;
}
