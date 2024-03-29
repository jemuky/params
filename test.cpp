
#include "param.h"

int main(int argc, char* argv[]) {
    Log::set_log_lev(LogLevel::Debug);

    params::Args args;
    using namespace params;
    args.set_rule("--exist", "", "", "", true);
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
    DEBUG("exist --exist?: ", args.exists({"--exist"}), "\texist 456?: ", args.exists({"456"}),
          "\texist 4564?: ", args.exists({"4564"}), "\texist daf?: ", args.exists({"daf"}))
    return 0;
}
