# C Toolbox

面向熟悉 C 的使用者，提供按需引入的 C99 字符串、容器和 IO。由 Annouimo/c-exam-kit 的工具箱开发分支迁入，原提交历史保留。

当前分支正在进行模块化迁移，尚未发布稳定版。旧脚手架请访问 https://github.com/Annouimo/c-exam-kit 。

## 使用

下载 Release 的单文件 Python 入口；运行后输入题目目录，再选择输入骨架。空输入导出 Markdown 手册。创建的 main.c 默认仅包含 ctb_io.h；字符串用 ctb_string.h，整数向量用 ctb_vec_int.h，其他类型按需引入。

每题目录自带 prepare.py，运行后扫描 C 文件、选择源文件、展开本地依赖、检查编译并复制提交源码。不会运行用户程序，除非指定 --run；不会覆盖原文件。每题保留创建时版本。

## 开发

```sh
python toolbox/build_headers.py
python toolbox/build.py
python toolbox/tests/test_modular.py
python toolbox/tests/test_toolbox.py
python toolbox/tests/test_workflow.py
```

新公共头文件在 toolbox/include；实现模板在 toolbox/templates。迁移状态与约定见 toolbox/docs/modular-migration.md。旧生成器暂作回归对照，旧脚手架当前目录已移除，完整历史保留。

## 最短用法

```c
#include "ctb_io.h"
int main(void) {
    int a = read_int();
    int b = read_int();
    print_ll((long long)a + b);
    print_newline();
    return 0;
}
```

字符串增加 ctb_string.h；IntVector 增加 ctb_vec_int.h。所有权、模块表和接口以[速查手册](toolbox/docs/quick_reference.md)为准。

## 发布

手动运行 Prepare Toolbox release draft 工作流；检查与打包通过后，仅在 main 创建预发布草稿，附件只有版本化 Python 脚本。发布前核对跨平台 CI 和附件，随后人工发布。已存在版本拒绝覆盖，修复时增加版本号；不提供 PDF 或可执行二进制。
