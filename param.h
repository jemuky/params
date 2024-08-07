#pragma once

#include "base.h"
#include "str_util.h"

namespace params {

struct Param {
    std::string lparam_;
    std::string sparam_;
    std::string help_text_;
    bool must_enter_;

    Param(const std::string& long_param, const std::string& short_param = "",
          const std::string& default_value = "", const std::string& help = "",
          bool must_enter = false)
        : lparam_(long_param),
          sparam_(short_param),
          help_text_(help),
          must_enter_(must_enter),
          __value_(default_value.size() == 0 ? nullptr : new std::string(default_value)) {
        assert(long_param.size() > 0);
    }
    ~Param() { delete __value_; }

    std::string help();

    template <class T,
              class = typename std::enable_if<std::is_same<T, std::vector<int>>::value>::type>
    bool get(std::vector<int>& t) {
        if (__value_ == nullptr) return false;
        auto item = split(*__value_, ",");
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
        if (__value_ == nullptr) return false;
        auto item = split(*__value_, ",");
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
        if (__value_ == nullptr) return false;
        t = split(*__value_, ",");
        return true;
    }

    template <class T, class = typename std::enable_if<std::is_same<T, bool>::value>::type>
    bool get(bool& t) {
        t = ((__value_ == nullptr || *__value_ == "false") ? false : true);
        return true;
    }

    template <typename T>
    bool get(T& t);

    void set_value(const std::string& str);

    bool check_rule();

    friend std::ostream& operator<<(std::ostream& out, const Param& self);

    bool operator==(const Param& rhs) const;

private:
    /// 处理bool时: 若set_rule a，但传递时未传参(xxx.exe --check)(传参: xxx.exe --check=false)会变成true，
    // 又因不想在set_value时设置类型，再者希望支持cpp11、14(不支持optional)，故此处value设为string ptr
    std::string* __value_;
};

struct KeyHash {
    std::size_t operator()(const Param& k) const;
};

class Args {
private:
    std::vector<Param*> __vparam_;
    std::string __exe_path_;
    std::string __app_name_;

    void __set_value(const std::string& lors_param, const std::string& value);

    bool __check_param(const Param& p);

    bool __check_rule();

public:
    ~Args();

    void usage();

    // 参数已存在返回false
    bool check_param(const std::string& long_param);

    /// 支持类型:
    /// bool: 无值或设置true为true，其他为false
    /// vector<int、double、string>: 使用,(逗号)分割
    /// 其余: 采用stringstream转
    /// 以上方法都不行返回失败
    template <typename T>
    bool get(const std::string& long_param, T& t);

    // 设置所需参数: 长参、短参、参数默认值(若长度为0不设默认值)、帮助信息、是否必传
    bool set_rule(const std::string& long_param, const std::string& short_param = "",
                  const std::string& default_value = "", const std::string& help = "",
                  bool must_enter = false);

    // 用=分隔
    bool analyze(int argc, char* argv[]);

    const std::string& get_exe_path();
    const std::string& get_app_name();

    // 参数是否在设置中存在(非参数列表)
    bool exists(const std::string& long_param);
};

template <typename T>
inline bool Param::get(T& t) {
    if (__value_ == nullptr) return false;
    return str_to_t(*__value_, t);
}

template <typename T>
inline bool Args::get(const std::string& long_param, T& t) {
    for (auto& param : __vparam_) {
        if (param->lparam_ == long_param) {
            if (!param->get<T>(t)) {
                return false;
            }
            return true;
        }
    }
    return false;
}

} // namespace params
