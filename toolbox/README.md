# C Toolbox v0.11.0 模块化工具箱

使用入口和开发命令见[仓库 README](../README.md)，接口见[速查手册](docs/quick_reference.md)，迁移决定见[迁移约定](docs/modular-migration.md)。

源码模板用于生成 public headers；单文件 Python 入口内嵌模块、提交准备脚本和手册。旧生成器仅保留作内部回归对照，不作为推荐使用入口。


### 单头文件模式

运行 `python toolbox/dist/c_toolbox_v0.11.0.py --folder 题目目录`，生成 `main.c`、`c_toolbox.h` 和 `prepare.py`。写题时引入完整头，提交时运行题目目录中的 `python prepare.py`。脚本使用内置索引组装提交文件，并报告 UTF-8 字节数。

默认模式面向单个 `main.c`，只接受工具箱和标准库的 `#include`，支持内置类型。自定义容器实例化和用户预处理宏请用 `--modular`。`c_toolbox.h` 和 `prepare.py` 由同一版本生成，应成对使用。
