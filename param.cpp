#include "param.h"

std::string params::Param::help() {
    std::string info = "  ";
    if (sparam_.size() > 0) {
        info.append(sparam_);
        info.push_back(',');
    }
    info.append(lparam_);
    info.append("\t");
    info.append(must_enter_ ? "necessary. " : "");
    info.append(help_text_);
    info.push_back('\n');
    return info;
}

void params::Param::set_value(const std::string& str) {
    __value_ = new std::string(std::move(str));
}

bool params::Param::check_rule() {
    if (must_enter_) {
        return __value_ != nullptr;
    }
    return true;
}

bool params::Param::operator==(const Param& rhs) const {
    return lparam_ == rhs.lparam_ || sparam_ == rhs.sparam_;
}

std::ostream& params::operator<<(std::ostream& out, const Param& self) {
    out << "{ long param:" << self.lparam_ << ", short param:" << self.sparam_
        << ", value:" << *self.__value_ << ", help:" << self.help_text_
        << ", must enter:" << self.must_enter_ << " }";
    return out;
}

std::size_t params::KeyHash::operator()(const Param& k) const {
    size_t h = 0;
    std::hash<std::string> hasher;
    h += hasher(k.lparam_);
    h += hasher(k.sparam_);
    return h;
}

void params::Args::__set_value(const std::string& lors_param, const std::string& value) {
    for (auto& param : __vparam_) {
        if (param->lparam_ == lors_param || param->sparam_ == lors_param) {
            param->set_value(value);
            break;
        }
    }
}

bool params::Args::__check_param(const Param& p) {
    for (const auto& param : __vparam_) {
        if (param->lparam_ == p.lparam_ || param->sparam_ == p.sparam_) return false;
    }
    return true;
}

bool params::Args::__check_rule() {
    for (const auto& v : __vparam_) {
        if (!v->check_rule()) {
            usage();
            return false;
        }
    }
    return true;
}

params::Args::~Args() {
    for (auto& pp : __vparam_) {
        if (pp) {
            delete pp;
        }
    }
}

void params::Args::usage() {
    std::string help_info = "\nUsage: " + __app_name_ +
                            " [bool argment] [bool argment]=[true/false] [argument]=[value]...\n";
    for (const auto& p : __vparam_) {
        help_info.append(p->help());
    }
    INFO(help_info);
}

bool params::Args::check_param(const std::string& long_param) {
    for (const auto& param : __vparam_) {
        if (param->lparam_ == long_param) return false;
    }
    return true;
}

bool params::Args::set_rule(const std::string& long_param, const std::string& short_param,
                            const std::string& default_value, const std::string& help,
                            bool must_enter) {
    if (!check_param(long_param)) return false;

    Param* p = new Param(long_param, short_param, default_value, help, must_enter);
    __vparam_.emplace_back(std::move(p));
    return true;
}

bool params::Args::analyze(int argc, char* argv[]) {
    assert(argc > 0);

    __exe_path_ = argv[0];
    auto index = __exe_path_.find_last_of("/\\");
    __app_name_ = index == std::string::npos ? __exe_path_ : __exe_path_.substr(index + 1);

    for (size_t i = 1; i < argc; ++i) {
        auto arg = split_two_part(argv[i], '=');
        __set_value(arg.first, arg.second);
    }
    return __check_rule();
}

const std::string& params::Args::get_exe_path() { return __exe_path_; }

const std::string& params::Args::get_app_name() { return __app_name_; }

bool params::Args::exists(const std::string& long_param) { return !check_param(long_param); }
