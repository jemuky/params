解析cpp命令行参数

# Usage

```cpp
#include "param.hpp"

using namespace params;

Args args;
// 设置参数限制(长参、短参、参数默认值(若长度为0不设默认值)、帮助信息、是否必传)
args.set_rule("--exist", "", "true", "", true);

// 使用debug打印--exist是否存在
DEBUG(args.exists("--exist"));
// 分析参数
if (!args.analyze(argc, argv)) {
    return 1;
}
// 使用v接收get获取的数据，get返回true说明解析成
bool v;
DEBUG(args.get("--exist", v), ", v:", v);
// 获取当前exe路径(包含app名)
DEBUG(args.get_exe_path());
// 获取exe名
DEBUG(args.get_app_name());
```