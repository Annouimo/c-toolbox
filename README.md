# C Toolbox

面向熟悉 C 的使用者，提供按需引入的 C99 字符串、容器和 IO。由 Annouimo/c-exam-kit 的工具箱开发分支迁入，原提交历史保留。

v0.9.0 增加字符与布尔容器，以及矩阵区域操作。旧脚手架请访问 https://github.com/Annouimo/c-exam-kit 。

## 使用

下载 Release 的单文件 Python 入口；运行后输入题目目录，再选择程序框架：空白 main、T 组、EOF、哨兵、q 次操作，或仅创建工具箱。空输入直接导出 Markdown 手册。输入类型由用户在循环中按题目修改，不单独提供成组的类型骨架。创建的 main.c 默认仅包含 `ctb/io.h`；字符串用 `ctb/string.h`，整数向量用 `ctb/vector/int.h`，其他类型按需引入。头文件放在题目目录的 `ctb/` 文件夹中。

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
#include "ctb/io.h"
int main(void) {
    int a = read_int();
    int b = read_int();
    print_ll((long long)a + b, "\n");
    return 0;
}
```

字符串增加 `#include "ctb/string.h"`；IntVector 增加 `#include "ctb/vector/int.h"`。所有权、模块表和接口以[速查手册](toolbox/docs/quick_reference.md)为准。

## 发布

合并到 main 后，自动运行 Linux GCC/Clang、macOS、Windows MinGW 和 Python 3.9 检查，构建唯一的版本化 Python 附件。新版本先创建草稿，上传并验证附件后自动发布正式 Release；失败保留草稿供检查。已发布版本自动跳过，不覆盖附件；修复时增加版本号。不提供 PDF 或可执行二进制。
