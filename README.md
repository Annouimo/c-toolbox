# C Toolbox

面向熟悉 C 的使用者，提供 C99 字符串、容器和 IO。由 Annouimo/c-exam-kit 的工具箱开发分支迁入，原提交历史保留。

v0.11.0 默认生成单头文件，提交时按需组装。旧脚手架请访问 https://github.com/Annouimo/c-exam-kit 。

## 使用

下载 Release 的单文件 Python 入口。运行后输入题目目录，再选择程序框架。空输入直接导出 Markdown 手册。默认生成 `main.c`、`c_toolbox.h` 和 `prepare.py`。选择“仅创建工具箱”可省略 `main.c`。

默认模式面向单个 `main.c`，支持内置 API，以及工具箱和标准库的 `#include`。运行 `prepare.py` 时，它读取 `main.c`，用内置索引组装单文件，报告字节数，编译通过后复制。完整头文件用于本地编译，须和 `prepare.py` 成对更新。

需要自定义容器、用户头文件或其他预处理指令时，用入口脚本的 `--modular` 生成分模块项目。该模式在提交时展开本地头文件。两种模式均可用 `--no-copy` 只检查和保存，或 `--run` 显式运行。每题保留创建时的版本。

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
#include "c_toolbox.h"
int main(void) {
    int a = read_int();
    int b = read_int();
    print_ll((long long)a + b, "\n");
    return 0;
}
```

所有权、模块表和接口以[速查手册](toolbox/docs/quick_reference.md)为准。

## 发布

合并到 main 后，自动运行 Linux GCC/Clang、macOS、Windows MinGW 和 Python 3.9 检查，构建唯一的版本化 Python 附件。新版本先创建草稿，上传并验证附件后自动发布正式 Release；失败保留草稿供检查。已发布版本自动跳过，不覆盖附件；修复时增加版本号。不提供 PDF 或可执行二进制。
