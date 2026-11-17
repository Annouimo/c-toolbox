# C Toolbox v0.8.0

- String 与 StringList 分别使用 `ctb/string.h` 和 `ctb/string_list.h`。
- Vector 与 Matrix 按元素类型从 `ctb/vector/` 和 `ctb/matrix/` 引入。三种数值矩阵共用模板，新增自定义 Matrix。保留 IndexVector。
- 移除两个无符号预制 Vector 和 Map、Set、Counter、NumericMap、ByteVector、Deque、Heap 的公开头文件。标量 IO 接口完整保留。
- 专用工具按用途归入 `ctb/math/` 和 `ctb/sequence/`，具体路径见速查手册。升级已有题目时需更新旧 include 路径。

下载 `c_toolbox_v0.8.0.py` 创建题目目录，目录内的 `prepare.py` 可展开新路径的头文件并检查提交源码。
