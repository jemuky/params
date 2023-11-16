#pragma once

#include "base.h"
#include "log.hpp"
#include "str_util.hpp"

namespace params {

struct __Param {
    std::string lparam_;
    std::string sparam_;
    std::string help_text_;
    bool must_enter_;

    __Param(const std::string& long_param, const std::string& short_param = "",
            const std::string& help = "", bool must_enter = false)
        : lparam_(long_param),
          sparam_(short_param),
          help_text_(help),
          must_enter_(must_enter),
          value_(nullptr) {
        assert(long_param.size() > 0);
    }
    ~__Param() {
        if (value_) {
            delete value_;
            value_ = nullptr;
        }
    }

    std::string help() {
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

    template <class T,
              class = typename std::enable_if<std::is_same<T, std::vector<int>>::value>::type>
    bool get(std::vector<int>& t) {
        if (value_ == nullptr) return false;
        auto item = split(*value_, ",");
        for (const auto& vv : item) {
            int v;
            if (!str_to_t(vv, v)) {
                return false;
            }
            t.emplace_back(v);
        }
        return true;
    }

    template <class T,
              class = typename std::enable_if<std::is_same<T, std::vector<double>>::value>::type>
    bool get(std::vector<double>& t) {
        if (value_ == nullptr) return false;
        auto item = split(*value_, ",");
        for (const auto& vv : item) {
            double v;
            if (!str_to_t(vv, v)) {
                return false;
            }
            t.emplace_back(v);
        }
        return true;
    }

    template <class T, class = typename std::enable_if<
                           std::is_same<T, std::vector<std::string>>::value>::type>
    bool get(std::vector<std::string>& t) {
        if (value_ == nullptr) return false;
        t = split(*value_, ",");
        return true;
    }

    template <class T, class = typename std::enable_if<std::is_same<T, bool>::value>::type>
    bool get(bool& t) {
        t = ((value_ == nullptr || *value_ == "false") ? false : true);
        return true;
    }

    template <typename T>
    bool get(T& t) {
        if (value_ == nullptr) return false;
        return str_to_t(*value_, t);
    }

    void set_value(const std::string& str) { value_ = new std::string(std::move(str)); }

    bool check_rule() {
        if (must_enter_) {
            return value_ != nullptr;
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& out, const __Param& self) {
        out << "{ long param:" << self.lparam_ << ", short param:" << self.sparam_
            << ", value:" << *self.value_ << ", help:" << self.help_text_
            << ", must enter:" << self.must_enter_ << " }";
        return out;
    }

    bool operator==(const __Param& rhs) const {
        return lparam_ == rhs.lparam_ || sparam_ == rhs.sparam_;
    }

private:
    /// 处理bool时: 若set_rule a，但传递时未传参(xxx.exe --check)(传参: xxx.exe --check=false)会变成true，
    // 又因不想在set_value时设置类型，再者希望支持cpp11、14(不支持optional)，故此处value设为string ptr
    std::string* value_;
};

struct __KeyHash {
    std::size_t operator()(const __Param& k) const {
        size_t h = 0;
        std::hash<std::string> hasher;
        h += hasher(k.lparam_);
        h += hasher(k.sparam_);
        return h;
    }
};
class Args {
private:
    std::vector<__Param*> vparam_;
    std::string exe_path_;
    std::string app_name_;

    void set_value(const std::string& lors_param, const std::string& value) {
        for (auto& param : vparam_) {
            if (param->lparam_ == lors_param || param->sparam_ == lors_param) {
                param->set_value(value);
                break;
            }
        }
    }

    bool check_param(const __Param& p) {
        for (const auto& param : vparam_) {
            if (param->lparam_ == p.lparam_ || param->sparam_ == p.sparam_) return false;
        }
        return true;
    }

    bool check_rule() {
        for (const auto& v : vparam_) {
            if (!v->check_rule()) {
                usage();
                return false;
            }
        }
        return true;
    }

public:
    Args() {}
    ~Args() {
        for (auto& pp : vparam_) {
            if (pp) {
                delete pp;
                pp = nullptr;
            }
        }
    }

    void usage() {
        std::string help_info =
            "\nUsage: " + app_name_ +
            " [bool argment] [bool argment]=[true/false] [argument]=[value]...\n";
        for (const auto& p : vparam_) {
            help_info.append(p->help());
        }
        INFO(help_info);
    }

    // 参数已存在返回false
    bool check_param(const std::string& long_param) {
        for (const auto& param : vparam_) {
            if (param->lparam_ == long_param) return false;
        }
        return true;
    }

    /// 支持类型:
    /// bool: 无值或设置true为true，其他为false
    /// vector<int、double、string>: 使用,(逗号)分割
    /// 其余: 采用stringstream转
    /// 以上方法都不行返回失败
    template <typename T>
    bool get(const std::string& long_param, T& t) {
        for (auto& param : vparam_) {
            if (param->lparam_ == long_param) {
                if (!param->get<T>(t)) {
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    // 设置所需参数: 长参、短参、帮助信息、是否必传
    bool set_rule(const std::string& long_param, const std::string& short_param = "",
                  const std::string& help = "", bool must_enter = false) {
        __Param* p = new __Param(long_param, short_param, help, must_enter);
        vparam_.emplace_back(std::move(p));
        return true;
    }

    // 用=分隔
    bool analyze(int argc, char* argv[]) {
        assert(argc > 0);

        exe_path_ = argv[0];
        auto index = exe_path_.find_last_of("/\\");
        app_name_ = index == std::string::npos ? exe_path_ : exe_path_.substr(index + 1);

        for (size_t i = 1; i < argc; ++i) {
            auto arg = split_two_part(argv[i], '=');
            set_value(arg.first, arg.second);
        }
        return check_rule();
    }
    const std::string& get_exe_path() { return exe_path_; }

    bool exists(const std::string& long_param) { return !check_param(long_param); }
};

} // namespace params
